"""
Toolkit for processing images downlinked from Flash using RC11 FPGA FW.
(now includes frame and line headers embedded in the byte stream).
Each image is composed of multiple interleaved frames.

@author: Connor W. Colombo (CMU)
@last-updated: 10/16/2023
"""
import re
import numpy as np
from scipy.interpolate import LinearNDInterpolator  # type: ignore
from scipy import ndimage as nd  # type: ignore
from dataclasses import dataclass, field
from collections import Counter, OrderedDict
from typing import Any, Final, Dict, Tuple, List, cast
import PIL.Image  # type: ignore

# Indices of Important Data in each Line Header:
FRAME_NUM_IDX: Final[int] = 0x03
FRAME_LINE_NUM_IDX: Final[int] = 0x07
INTER_LINE_NUM_IDX: Final[int] = 0x0B

# Regex for line headers:
# Example / dummy line header where all embedded data is 0x00:
BLANK_HEADER: Final = b'\x11\x22\x33\x00\x55\x66\x77\x00\x99\xAA\xBB\x00\xDD\xEE\xFF'
# (FPGA replaces the first few bytes of each line with an identifiable header that has line metadata embedded in it):
LINE_HEADER_REGEX: Final = b'\x11\x22\x33.{1}\x55\x66\x77.{1}\x99\xAA\xBB.{1}\xDD\xEE\xFF'
# Regex to find the line headers:
LINE_HEADERS_PATTERN: Final = re.compile(LINE_HEADER_REGEX)
# Regex to find all matches between headers:
LINE_CONTENTS_PATTERN: Final = re.compile(
    b'(?<='+LINE_HEADER_REGEX + b')([\S\s]*?)' + LINE_HEADER_REGEX)

# Regex for frame headers:
# (FPGA hijacks the first line of each frame for frame-metadata):
FRAME_HEADER_DATA_REGEX = b"IRIS-FPGA-FW--RC.{1}\..{1}\..{1}--SENSOR-FRAME-NUM:.{1}--INTERLEAVED-FRAME-NUM:.{1}--IMAGING-MODE:.{1}--"
# frame_header_gradient_regex = b".?(\x00.)+"  # a repeating pattern of x00 then anything (starts on either the x00 or the anything)
# Frame header is a combination of the data part and the gradient part, followed by a normal line header
# ... but since the gradient presently appears to be interrupted by that page jump,
# we're just going to say its a frame header data section and whatever bytes follow until the next line header
FRAME_HEADER_PATTERN = re.compile(
    b'('+FRAME_HEADER_DATA_REGEX + b'[\S\s]*?)' + LINE_HEADER_REGEX)

# Build a map from (interleave_frame_idx, line_idx_in_interleaved_frame) to a
# line number in an image.
# This is a fixed thing and isn't a function of any image data, so we can just
# build this up front:

N_INTERLEAVED_FRAMES: Final[int] = 8
LINES_IN_FRAME: Final[int] = 1944


def build_image_line_map(
    # Number of interleaved frames per image:
    # To perform correctly, it should be a factor of `LINES_IN_FRAME`.
    N_INTERLEAVED_FRAMES: int = N_INTERLEAVED_FRAMES,
    # Number of lines in each full frame
    LINES_IN_FRAME: int = LINES_IN_FRAME
) -> Dict[Tuple[int, int], int]:
    INTERLEAVE_JUMP_SIZE: int = 2*N_INTERLEAVED_FRAMES - 1
    INTERLEAVED_LINES_PER_FRAME: int = LINES_IN_FRAME // N_INTERLEAVED_FRAMES
    # Use a translation of state machine in `camera_interface.v` to create a
    # map: (based on "Iris FPGA FW Notes.xlsx", which )
    line_in_int_frame = [0]
    int_frame_idx = [0]
    line_in_image = [0]  # "target line"
    for _ in range(LINES_IN_FRAME-1):
        # Calculate updates:
        if line_in_int_frame[-1] == (INTERLEAVED_LINES_PER_FRAME-1):
            next_line_in_int_frame = 0
            next_int_frame_idx = int_frame_idx[-1]+1
        else:
            next_line_in_int_frame = line_in_int_frame[-1] + 1
            next_int_frame_idx = int_frame_idx[-1]

        starting_new_int_frame = (
            line_in_int_frame[-1] >= (INTERLEAVED_LINES_PER_FRAME-1) and
            int_frame_idx[-1] < (N_INTERLEAVED_FRAMES-1)
        )
        prev_target_is_even = line_in_image[-1] % 2 == 0
        if starting_new_int_frame:
            next_line_in_image = 2 * next_int_frame_idx
        # Bayer-preserving interleave pattern:
        elif prev_target_is_even:
            next_line_in_image = line_in_image[-1] + 1
        else:
            if line_in_image[-1] < LINES_IN_FRAME-INTERLEAVE_JUMP_SIZE:
                next_line_in_image = line_in_image[-1] + INTERLEAVE_JUMP_SIZE
            else:
                next_line_in_image = line_in_image[-1] + \
                    INTERLEAVE_JUMP_SIZE - N_INTERLEAVED_FRAMES + 1
        # Apply Updates:
        line_in_int_frame.append(next_line_in_int_frame)
        int_frame_idx.append(next_int_frame_idx)
        line_in_image.append(next_line_in_image)
    # Build map:
    return {
        (frame_num, int_line_num): image_line_num
        for (frame_num, int_line_num, image_line_num)
        in zip(int_frame_idx, line_in_int_frame, line_in_image)
    }


image_line_map = build_image_line_map()


@dataclass
class FrameMetadataRc11:
    # All reported firmware versions with associated counts (should only be one
    # and its count should match the number of frame headers present):
    fw_versions: Dict[str, int]
    # All Sensor Frame Numbers reported, in order:
    sensor_frame_nums: List[int]
    # Interleaved Frame Numbers reported, in order:
    interleaved_frame_nums: List[int]
    # Imaging Modes reported, in order:
    imaging_mode: Dict[Any, int]


@dataclass
class ImageDataRc11:
    """All the processed raw data representing an RC11-style image."""
    # Metadata about all the frames in the image:
    frame_metadata: FrameMetadataRc11
    # List of all lines found in the image, in the order they appeared in
    # flash:
    _found_lines: List[bytes]
    # Maximum length for a line:
    _max_line_size: int
    # Filtered version of found_lines, only include those with sensible metadata:
    _found_lines_filtered: List[bytes] = field(init=False)
    # Map for a line idx from interleaved frame space to image space:
    _ordered_frame_nums_map: OrderedDict[int, int] = field(init=False)
    # Cache of all built frames:
    _built_frames_cache: List[np.ndarray] = field(default_factory=lambda: [])

    def __post_init__(self):
        self._found_lines_filtered = self._filter_found_lines()
        self._ordered_frame_nums_map = self._build_frame_num_map()

    @property
    def frame_indices(self) -> List[int]:
        # All valid frame indices
        return [*self._ordered_frame_nums_map.values()]

    def build_raw_image(self, cap_line_length: bool = True) -> np.ndarray:
        # Converts the found lines into a raw image
        # (i.e. order they appeared in flash but aligned correctly)
        # Make a copy:
        lines_data = [x for x in self._found_lines]

        # Exclude any lines that are too long (i.e. giant chunk of bytes at the end):
        # If the last line is too long, trim it (giant chunk of bytes at the end that
        # didn't sequence properly):
        lines_data[-1] = lines_data[-1][:self._max_line_size]

        if cap_line_length:
            lines_data = [line[:self._max_line_size] for line in lines_data]

        # Pad all lines out to the same length:
        pad_byte = b'\xAA'
        max_len = max(len(line_data) for line_data in lines_data)
        for i, line_data in enumerate(lines_data):
            size = len(line_data)
            diff = max_len - size
            if diff > 0:
                lines_data[i] = line_data + pad_byte*diff

        lines_arr = [[int(x) for x in l] for l in lines_data]
        return np.array(lines_arr, dtype=np.uint8)

    def report_all_line_metadata(self) -> str:
        # Creates a report containing all the metadata about each line:
        # (a legacy debugging tool)
        report = "Idx \t NFrame\t NFLine\t NILine\t Header\t\t\t\t\t\t Start Of Data"
        for idx, line_data in enumerate(self._found_lines):
            line_header = line_data[:15]
            line_header_blank = all(
                x == 0x00 or x == 0xFF for x in line_header)
            if not line_header_blank:
                line_header_txt = ':'.join(f'{x:02X}' for x in line_header)
                # Extract data from line header:
                frame_num = line_header[FRAME_NUM_IDX]
                frame_line_num = line_header[FRAME_LINE_NUM_IDX]
                interleaved_frame_line_num = line_header[INTER_LINE_NUM_IDX]
                report += (
                    f"{idx:5d}:\t "
                    f"{frame_num},\t "
                    f"{frame_line_num},\t "
                    f"{interleaved_frame_line_num},\t "
                    f"{line_header_txt} \t "
                    + ':'.join(f'{x:02X}' for x in line_data[15:20])
                )
        return report

    def _filter_found_lines(self) -> List[bytes]:
        # Creates a copy of found_lines that excludes any nonsensical lines.
        # Only needs to be run once on post_init.
        # Grab every frame number:
        reported_frame_nums = [line[FRAME_NUM_IDX]
                               for line in self._found_lines]
        # Count number of lines we captured per frame number:
        frame_num_counts = Counter(reported_frame_nums)
        # Only keep frame numbers with more than 8 lines (arbitrary), so we don't count
        filtered_frame_nums = [fn for fn,
                               count in frame_num_counts.items() if count > 8]

        # Only keep lines with a validated frame number:
        return [
            line for line in self._found_lines
            if line[FRAME_NUM_IDX] in filtered_frame_nums
        ]

    def _build_frame_num_map(self) -> OrderedDict[int, int]:
        # Build a map from sensor frame numbers to interleaved frame index.
        # Only needs to be run once on post_init.
        # *`_filter_found_lines` MUST BE RUN FIRST*

        # Map those frame numbers to interleaved frame indices
        # (i.e. frame nums might be: [11, 12, 15, 16, 17, 18, 22, 24] and we'd need
        # to have a map that says these are interleaved frames: [0,1,2,3,4,5,6,7])
        #
        # Make sure these appear in the order they appear in the image (note: sorting
        # isn't sufficient here b/c frame numbers are basically just a count of
        # (synchronized, stabilized, and denoised) FV since boot, so it may wrap around
        # - e.g. frame nums may be [249, 250, 253, 254, 0, 1, 4, 5])
        # NOTE: This is more susceptible to noise, might want to switch to just
        # incrementing the "frame number" used here when line number resets since
        # that's computed a lot more robustly.
        #
        # Maps a frame_num (key) to the corresponding interleaved frame index (value):
        ordered_frame_nums_map: OrderedDict[int, int] = OrderedDict()
        for line in self._found_lines_filtered:
            fn = line[FRAME_NUM_IDX]
            if fn not in ordered_frame_nums_map:
                ordered_frame_nums_map[fn] = len(ordered_frame_nums_map)
        return ordered_frame_nums_map

    def _line_to_line_index(self, line: bytes) -> int | None:
        # Returns the line index in the image where this should go based on the
        # line header or `None` if the line info is invalid.
        reported_frame_number = line[FRAME_NUM_IDX]
        if reported_frame_number not in self._ordered_frame_nums_map:
            # Invalid Frame Number
            # we can't know where this line goes so just ignore it
            # (this should be rare and interp will fill it in)
            return None
        interleaved_frame_num = self._ordered_frame_nums_map[reported_frame_number]
        interleaved_frame_line_num = line[INTER_LINE_NUM_IDX]
        interleaved_index = (interleaved_frame_num, interleaved_frame_line_num)
        if interleaved_index not in image_line_map:
            # Invalid interleaved frame number or interleaved line number
            # we can't know where this line goes so just ignore it
            # (this should be rare and interp will fill it in)
            return None
        image_line_index = image_line_map[interleaved_index]
        return image_line_index

    def smart_build(
        self,
        interpolate_unknown: bool = True,
        # Whether or not to use fast nearest neighbor if interp isn't or can't be used:
        use_nearest_if_no_interp: bool = True,
        pad_byte: bytes = b'\x00',  # Easier to see with 0x00 than 0xAA or 0xFF
        idx_x_seam: int = 0,
        idx_y_seam: int = 0,
        keep_frame_indices: List[int] | None = None
    ) -> np.ndarray:
        # Build an image using only cleaned found lines,
        # reconstructing using line headers, interpolating if desired.
        full_shape = (LINES_IN_FRAME, self._max_line_size)
        image: np.ma.MaskedArray = np.ma.MaskedArray(
            data=np.zeros(full_shape, dtype=np.uint8),
            mask=True
        )
        image_out: np.ndarray

        # For each line extracted from the file vector:
        for line in self._found_lines_filtered:
            # If `keep_frame_indices` is specified, don't use any frame indices
            # that aren't in that list:
            reported_frame_number = line[FRAME_NUM_IDX]
            if reported_frame_number not in self._ordered_frame_nums_map:
                continue
            interleaved_frame_idx = self._ordered_frame_nums_map[reported_frame_number]
            if keep_frame_indices is not None and interleaved_frame_idx not in keep_frame_indices:
                continue

            # Keep only first line's worth of data from each line:
            data_to_write = line[:full_shape[1]]

            # Write line data to the appropriate region of the image based on the
            # metadata in the line header (or skip if we can't determine that):
            image_line_index = self._line_to_line_index(data_to_write)
            if image_line_index is None:
                continue  # Skip. Don't add this line to the image
            image[image_line_index, 0:len(data_to_write)] = [
                int(x) for x in data_to_write
            ]

        interpolation_failed: bool = False
        if interpolate_unknown:
            try:
                # NOTE: This can likely be a lot more efficient, however this
                # works very well and in sufficient time compared to the
                # timescale of a downlink.
                X, Y = np.mgrid[0:image.shape[0], 0:image.shape[1]]
                valid = ~image.mask  # has data
                interpolator = LinearNDInterpolator(
                    list(zip(X[valid], Y[valid])), image[valid]
                )
                image_out = interpolator(X, Y)
            except Exception as e:
                print(
                    f"Interim Image interpolation failed b/c `{e}`. "
                    f"Defaulting to fill with `{pad_byte[0]!r}`."
                )
                interpolation_failed = True

        # If interpolation fails or is not used:
        if not interpolate_unknown or interpolation_failed:
            if use_nearest_if_no_interp:
                # Fill unknowns with Nearest Neighbor:
                invalid = image.mask
                ind = nd.distance_transform_edt(
                    invalid, return_distances=False, return_indices=True
                )
                image_out = image[tuple(ind)]
            else:
                # Fill unknowns with Pad Byte:
                image_out = image.filled(int(pad_byte[0]))

        # Apply seam fixes (shouldn't be necessary any more):
        image_out = np.roll(
            image_out, (-idx_y_seam, -idx_x_seam), axis=(0, 1)
        )

        return image_out

    def quick_build(self, **kwargs) -> np.ndarray:
        # Quickly assemble an image, only lines received, w/out interpolation,
        # w/NN-approximation:
        return self.smart_build(
            interpolate_unknown=False,
            use_nearest_if_no_interp=True,
            **kwargs
        )

    def build_frame(self, frame_idx: int, interp: bool = True, **kwargs) -> np.ndarray:
        # Builds the frame with the given index.
        # Returns a cached result if called multiple times.
        # Optionally, interpolates all missing lines (which will otherwise be
        # colored solid). Slows down the operation but makes the frames much
        # more legible. See `smart_build` for more details.
        return self.smart_build(
            interpolate_unknown=interp,
            keep_frame_indices=[frame_idx],
            **kwargs
        )

    def build_or_fetch_all_frames(self) -> List[np.ndarray]:
        # Builds all frames or fetches them from cache if they've already been
        # built.
        # Uses interp exclusively since almost always that will be desired for
        # interleaved frames since most of their lines are blank.
        if len(self._built_frames_cache) < len(self.frame_indices):
            # Some or all of the frames haven't been built yet,
            # we need to build them:
            self._built_frames_cache = [
                self.build_frame(i, interp=True)
                for i in self.frame_indices
            ]
        return self._built_frames_cache

    def build_median_frame(self) -> np.ndarray:
        # Takes a median of a stack of all (interpolated) frames.
        # Used as a technique to decrease noise, particularly in overexposed
        # areas.
        frames = self.build_or_fetch_all_frames()
        shape = frames[0].shape
        frame_stack = np.vstack([
            f.reshape(1, shape[0]*shape[1]) for f in frames
        ]).copy()  # copy to avoid altering the cached frames

        # Remove pixels that seem to be from unwritten sections of Flash (exactly 0xFF):
        frame_stack = frame_stack.astype(np.float32)
        frame_stack[frame_stack == 0xFF] = np.NaN

        # Mean and Median them:
        median_of_frames = np.nanmedian(frame_stack, axis=0).reshape(*shape)

        # Replace NaNs in median/mean images with 0xFF (for consistency):
        median_of_frames[np.isnan(median_of_frames)] = 0xFF
        return cast(np.ndarray, median_of_frames)

    def save_frames_gif(
        self,
        path: str,
        rotation: int = 0,
        unknown_cutoff: float = 0.25
    ) -> None:
        # Save a gif of all frames to path. Only includes frames with less than
        # `unknown_cutoff` (0 to 1.0) of its pixels unknown.
        frames = self.build_or_fetch_all_frames()

        # Percent NaN of each Frame (how much of each frame is unknown):
        frame_unknown_percent = [
            np.count_nonzero(f == 0xFF)/np.size(f) for f in frames
        ]

        # Convert frames back to uint8 after replacing all NaN with 0xFF again:
        # Only include frames where at least 75% of the pixels are known (not nan):
        gif_frames = [
            f.copy() for f, p_unk in zip(frames, frame_unknown_percent)
            if p_unk < unknown_cutoff
        ]

        # Convert to PIL Image and Save:
        pil_frames = [
            PIL.Image.fromarray(gf.astype('uint8'), 'L').rotate(rotation)
            for gf in gif_frames
        ]  # L = 8bit, grayscale
        pil_frames[0].save(
            path,
            save_all=True,
            append_images=pil_frames[1:],
            duration=100,
            interlaced=False,
            loop=0
        )  # loop 0 = forever


def extract_image_from_flash_memory(
    flash_bytes: bytes,
    max_line_size: int = 2592,
    # Settings for removing unwritten (white / 0xFF) sections of memory from each row:
    remove_white: bool = True,
    # Nominal size of white band:
    remove_white_size: int = 512-32,
    # Tolerance of white band size (due to binning), default is max usable bin size:
    remove_white_tol: int = 16
) -> ImageDataRc11:
    """Extracts RC11-style image data from a continuous vector of bytes
    downlinked from the flash memory. Automatically recognizes frames, aligns
    lines, etc. Everything needed to extract the data for a well-formatted
    image."""

    # Extract Frame Metadata Lines:
    frame_headers = FRAME_HEADER_PATTERN.findall(flash_bytes)
    # Extract data from the frame headers:
    fw_ver_major = [fh[16] for fh in frame_headers]
    fw_ver_minor = [fh[18] for fh in frame_headers]
    fw_ver_patch = [fh[20] for fh in frame_headers]
    fw_versions = [f"RC{maj:02d}.{min:02d}.{patch:02d}" for (
        maj, min, patch) in zip(fw_ver_major, fw_ver_minor, fw_ver_patch)]
    sensor_frame_nums = [fh[40] for fh in frame_headers]
    interleaved_frame_nums = [fh[65] for fh in frame_headers]
    imaging_mode = [fh[81] for fh in frame_headers]

    # Summarize frame metadata, using a counter to look for consensus where each
    # frame header should contain the same data:
    reported_frame_metadata = FrameMetadataRc11(
        fw_versions=Counter(fw_versions),
        sensor_frame_nums=sensor_frame_nums,
        interleaved_frame_nums=interleaved_frame_nums,
        imaging_mode=Counter(imaging_mode)
    )

    # Remove Frame Metadata Lines:
    flash_bytes_no_frame_header = FRAME_HEADER_PATTERN.sub(b'', flash_bytes)

    # Brute-Force Find all Image Lines in Flash Vector using Line Headers:
    # Search in vec with blank header tacked on the end so we can capture the last line too:
    found_lines = [header + body for header, body in zip(
        LINE_HEADERS_PATTERN.findall(flash_bytes_no_frame_header),
        LINE_CONTENTS_PATTERN.findall(
            flash_bytes_no_frame_header + BLANK_HEADER)
    )]

    # If desired, remove any white (exactly 0xFF) sections from each row that
    # are the same size as the blank region at the end of each memory page
    # (not actually part of an image):
    if remove_white:
        unwritten_pattern = (
            b'\xFF{' +
            str(int(remove_white_size-remove_white_tol)).encode('utf-8') +
            b',' +
            str(int(remove_white_size+remove_white_tol)).encode('utf-8') +
            b'}'
        )
        found_lines = [
            re.sub(unwritten_pattern, b'', fl)
            for fl in found_lines
        ]

    return ImageDataRc11(
        frame_metadata=reported_frame_metadata,
        _found_lines=found_lines,
        _max_line_size=max_line_size
    )
