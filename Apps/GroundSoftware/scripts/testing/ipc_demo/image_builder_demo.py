"""
Starting implementation of an IPC file builder.

Run all GDS infrastructure before running this:
```
make run-infra
```

Also run a Transceiver to supply packets (e.g. PcapTransceiver).

The following is an example of a way to feed data from a wirelessly captured
PCAP using a PcapTransceiver:
```
./run-script.sh scripts/testing/ipc_demo/pcap_xcvr.py -t 10 -g 0 -l VERBOSE --no-loop -f test-data/img_image-grid-flash-wifi_test_rebootAfter6292.pcap
```

@author: Connor W. Colombo (CMU)
@last-updated: 10/21/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

import os
import cv2  # type: ignore
import ulid
import pickle
import os.path
import argparse
from functools import lru_cache
from dataclasses import dataclass, field
from itertools import zip_longest
from collections import OrderedDict, Counter
from typing import Any, Dict, List, Tuple, cast, TypeAlias, Type, Final, Callable

import numpy as np
from scipy.interpolate import LinearNDInterpolator  # type: ignore

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket
from IrisBackendv3.codec.payload import FileBlockPayload, FileMetadata
from IrisBackendv3.ipc.messages import (
    DownlinkedPayloadsMessage
)
IB3.init_from_latest()

app = ipc.IpcAppHelper("FileBuilderDemo")
app.setLogLevel('VERBOSE')

# Where output files get saved (w.r.t GroundSoftware)
OUTPUT_DIR_ROOT: Final[str] = './out/image_file_decodings/'

# Prefix to apply to all files:
FILE_PREFIX_DEFAULT: Final[str] = 'RC9_Test_Img'

# Default number of lines in an image:
DEFAULT_IMAGE_LINE_COUNT: Final[int] = 1944
DEFAULT_IMAGE_WIDTH: Final[int] = 512*6  # 2592  (all 6 pages now)

FileIdType: TypeAlias = int

# Setup:
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    )
})


@dataclass
class ImageLine:
    # Metadata for this image line ("File"):
    metadata: FileMetadata | None
    # Growing list of all file block payloads found, **in the order found**:
    data_block_payloads: List[FileBlockPayload]
    # Fixed list of all file block payloads in this line (length stays fixed at
    # `totalBlocks`)
    ordered_data_blocks: List[bytes]

    def __str__(self) -> str:
        return (
            "ImageLine["
            f"line: {0 if self.line_num is None else self.line_num:4d},\t "
            f"fgid: {0 if self.file_group_id is None else self.file_group_id:6d},\t "
            f"{len(self.data_block_payloads):2d} blocks,\t "
            f"{len(self.ordered_data_blocks):2d} allocd,\t "
            f"{sum(1 for x in self.ordered_data_blocks if x != b''):2d} used,\t "
            f"nbf: {self.num_blocks_found:2d},\t "
            f"tnb: {self.total_num_blocks:2d},\t "
            f"empt: {'y' if self.is_empty else 'n'},\t "
            f"comp: {'y' if self.is_complete else 'n'},\t "
            f"corr: {'y' if self.contains_corrupted_blocks() else 'n'},\t "
            f"val: {'y' if self.validate() else 'n'},\t "
            f"meta: {self.metadata!s}"
            "]"
        )

    @property
    def num_blocks_found(self) -> int:
        return len(self.data_block_payloads)

    @property
    def total_num_blocks(self) -> int:
        return len(self.ordered_data_blocks)

    @property
    def is_complete(self) -> bool:
        # TODO: Cache this based on num blocks received (won't change if new blocks don't come in)
        # Using >= not == b/c sometimes we get duplicate blocks:
        return (
            # Make sure we've received enough blocks:
            self.num_blocks_found >= self.total_num_blocks
            # And no allocated sections are empty:
            # (have to check this because it's possible we've received a
            # multiple copies of one block, inflating the count):
            and not any(x == b'' for x in self.ordered_data_blocks)
        )

    @property
    def line_num(self) -> int | None:
        """0-indexed line number of this line in the File Group (Image)."""
        if len(self.data_block_payloads) == 0:
            # No data yet.
            return None

        # Check for consensus:
        b0_num = self.data_block_payloads[0].file_group_line_number
        if any(b.file_group_line_number != b0_num for b in self.data_block_payloads):
            # Find most common value:
            nums = [b.file_group_line_number for b in self.data_block_payloads]
            most_common = Counter(nums).most_common(1)[0][0]
            return most_common
        else:
            return b0_num

    @property
    def file_group_id(self) -> int | None:
        """Hashed ID of the File Group (Image) this line belongs to."""
        if len(self.data_block_payloads) == 0:
            # No data yet.
            return None

        # Check for consensus:
        b0_fg_id = self.data_block_payloads[0].file_group_id
        if any(b.file_group_id != b0_fg_id for b in self.data_block_payloads):
            # Find most common value:
            fg_ids = [b.file_group_id for b in self.data_block_payloads]
            most_common = Counter(fg_ids).most_common(1)[0][0]
            return most_common
        else:
            return b0_fg_id

    def validate_line_num(self) -> bool:
        """Checks if reported line numbers are internally consistent within
        this line."""
        if len(self.data_block_payloads) == 0:
            # No data yet.
            return True
        # Check for consensus:
        b0_num = self.data_block_payloads[0].file_group_line_number
        if any(b.file_group_line_number != b0_num for b in self.data_block_payloads):
            # Report the disagreement and resolution:
            nums = [b.file_group_line_number for b in self.data_block_payloads]
            app.logger.warning(
                f"For `ImageLine` with metadata: `{self.metadata}`: "
                f"Disagreement about `file_group_line_number`. "
                f"Counts of values reported by blocks in file line are: "
                f"`{Counter(nums)}`. "
                f"Using value: `{self.line_num = }`."
            )
            return False
        else:
            return True

    def validate_file_group_id(self) -> bool:
        """Checks if reported `file_group_id` is internally consistent within
        this line."""
        if len(self.data_block_payloads) == 0:
            # No data yet.
            return True
        # Check for consensus:
        b0_fg_id = self.data_block_payloads[0].file_group_id
        if any(b.file_group_id != b0_fg_id for b in self.data_block_payloads):
            # Report the disagreement and resolution:
            fg_ids = [b.file_group_id for b in self.data_block_payloads]
            app.logger.warning(
                f"For `ImageLine` with `line_num = {self.line_num}` "
                f"and metadata: `{self.metadata}`: "
                f"Disagreement about `file_group_id`. "
                f"Counts of values reported by blocks in file line are: "
                f"`{Counter(fg_ids)}`. "
                f"Using value: `{self.file_group_id = }`."
            )
            return False
        else:
            return True

    def validate_total_blocks(self) -> bool:
        """
        Makes sure all blocks agree on total block count (should match space
        allocated for data blocks, which should have come from
        `b.total_blocks` of one payload).
        """
        blocks_valid = all(
            b.total_blocks == self.total_num_blocks
            for b in self.data_block_payloads
        )
        if not blocks_valid:
            app.logger.warning(
                f"For `ImageLine` with `line_num = {self.line_num}` "
                f"and metadata: `{self.metadata}`: "
                f"All blocks in line should agree on total blocks. "
                f"Space allocated for blocks: `{self.total_num_blocks=}`. "
                f"Instead, blocks say: "
                f"{[b.total_blocks for b in self.data_block_payloads]}"
            )
        return blocks_valid

    def contains_corrupted_blocks(self) -> bool:
        """Checks if any of the file blocks in this line are possibly
        corrupted (contain internal inconsistencies)."""
        possibly_corrupted_lines = [
            b for b in self.data_block_payloads
            if b.possible_corruption
        ]
        if len(possibly_corrupted_lines) > 0:
            app.logger.warning(
                f"For `ImageLine` with `line_num = {self.line_num}` "
                f"and metadata: `{self.metadata}`: "
                f"contains the following possibly corrupted lines: "
                f"{possibly_corrupted_lines}."
            )
        return len(possibly_corrupted_lines) > 0

    @property
    def is_empty(self) -> bool:
        """Returns whether this `ImageLine` contains no data."""
        return (
            # No non-metadata blocks received yet:
            len(self.data_block_payloads) == 0 and
            # No data in blocks (*should* be equivalent to the prev. condition):
            sum(1 for x in self.ordered_data_blocks if x != b'') == 0
        )

    def validate(self) -> bool:
        """Returns whether this `ImageLine` is valid & internally consistent."""
        # Collect as bools first so ALL validation checks are run
        # (this allows them to emit logs as needed)
        line_num_good = self.validate_line_num()
        fg_id_good = self.validate_file_group_id()
        total_blocks_good = self.validate_total_blocks()
        no_corruption = not self.contains_corrupted_blocks()
        return (
            line_num_good and
            fg_id_good and
            total_blocks_good and
            no_corruption
        )

    def assemble(self) -> bytes:
        """Assembles all the blocks in this `ImageLine` into one byte string
        representing the line."""
        if len(self.ordered_data_blocks) == 0:
            return b''
        return b''.join(self.ordered_data_blocks)

    @staticmethod
    def block_num_to_idx(block_num: int) -> int:
        """Convert 1-indexed block_num to 0-indexed data index for
        `ordered_data_blocks`. Currently, block numbers for data start at 1
        because 0 is reserved for metadata.
        """
        return block_num-1

    def add_block(self, block: FileBlockPayload) -> None:
        """Adds the given `FileBlockPayload` to this `ImageLine`.
        Adjusts `ordered_data_blocks` if necessary (only necessary if
        `total_blocks` was corrupted in a previous block).
        """
        block_idx = self.block_num_to_idx(block.block_number)
        total_blocks = block.total_blocks
        # Determine what size this block thinks this line should be (make a
        # best guess in case it's corrupted). Min is 1:
        target_size = max(total_blocks, block_idx+1, 1)

        # Shrink `total_blocks` if necessary (and possible):
        if target_size < self.total_num_blocks:
            # Only allow shrinking if no data has been written past the
            # threshold proposed by this block:
            shrunken = self.ordered_data_blocks[target_size:]
            can_shrink = all(d == b'' for d in shrunken)
            if can_shrink:
                self.ordered_data_blocks = shrunken

        # Grow `total_blocks` if necessary:
        if target_size > self.total_num_blocks:
            diff = target_size - self.total_num_blocks
            self.ordered_data_blocks += [b'']*diff

        # If this block is metadata, just handle it as metadata:
        if block.is_metadata:
            self.metadata = block.file_metadata
        else:
            # This is a data block:
            # Append this block to the list of blocks collected:
            self.data_block_payloads.append(block)
            # Add this block's data to the right part of the line:
            self.ordered_data_blocks[block_idx] = block.data

    @classmethod
    def new_empty(cls: Type[ImageLine]) -> ImageLine:
        """Initializes an empty `ImageLine`."""
        return cls(
            metadata=None,
            data_block_payloads=[],
            ordered_data_blocks=[]
        )

    @classmethod
    def from_first_block(cls: Type[ImageLine], block: FileBlockPayload) -> ImageLine:
        """Initializes this `ImageLine` from whatever `FileBlockPayload` was
        found first (should be `FileMetadata` but that's not guaranteed).
        NOTE: FileMetadata is now only downlinked for the first line,
        last line, and any lines where %100==0.
        """
        line = cls.new_empty()
        # `add_block` automatically handles sizing the line, so we don't have
        # to worry about it here...
        line.add_block(block)
        if not block.is_metadata and (block.file_group_line_number % 100 == 0):
            app.logger.warning(
                f"First block for `ImageLine` with "
                f"line={block.file_group_line_number} in {block.file_group_id} "
                f"was not `FileMetadata`. "
                "Metadata should be in every line with (fg_line_number%100==0). "
                "Missing it on the first block is unusual but not impossible."
            )
        return line


def justin_algorithm(
    mem: np.ndarray,
    n_interleave: int = 8
) -> Dict[str, np.ndarray]:
    """Uses Justin's algorithm from `image_from_pcap.py` to convert an image
    from memory space to pixel space."""
    # Interleave rows:
    np_img: np.ndarray  # working image. result of each stage
    results: Dict[str, np.ndarray] = dict()
    img_int = []
    for i in range(int(mem.shape[0] / n_interleave)):
        for n in range(n_interleave):
            img_int.append(mem[int(mem.shape[0] / n_interleave * n + i)])
    np_img = np.array(img_int)
    results['interleaved'] = np_img

    # # Rearrange the four quadrants:
    # split_y = int(1748.0 * mem.shape[0] / DEFAULT_IMAGE_LINE_COUNT)
    # split_x = int(1878.0 * mem.shape[1] / DEFAULT_IMAGE_WIDTH)
    # br = np_img[:split_y, :split_x]
    # tr = np_img[split_y:, :split_x]
    # bl = np_img[:split_y, split_x:]
    # tl = np_img[split_y:, split_x:]
    # picture: np.ndarray = np.ndarray(mem.shape, dtype=np.uint8)
    # picture[:tl.shape[0], :tl.shape[1]] = tl
    # picture[:tr.shape[0], tl.shape[1]:] = tr
    # picture[tr.shape[0]:, tl.shape[1]:] = br
    # picture[tl.shape[0]:, :tl.shape[1]] = bl
    # np_img = picture
    # results['rearranged'] = np_img

    # Get rid of every 8th row (all the white rows)
    pic = []
    for i, row in enumerate(np_img):
        if i % 8 == 7:
            pass
        else:
            pic.append(row)
    np_img = np.array(pic)
    results['pruned'] = np_img

    # Stretch pruned image back to the original size:
    results['pruned-stretched-nearest'] = cv2.resize(
        results['pruned'],
        (mem.shape[1], mem.shape[0]),
        interpolation=cv2.INTER_NEAREST
    )
    results['pruned-stretched-linear'] = cv2.resize(
        results['pruned'],
        (mem.shape[1], mem.shape[0]),
        interpolation=cv2.INTER_LINEAR
    )
    results['pruned-stretched-cubic'] = cv2.resize(
        results['pruned'],
        (mem.shape[1], mem.shape[0]),
        interpolation=cv2.INTER_CUBIC
    )
    results['pruned-stretched-area'] = cv2.resize(
        results['pruned'],
        (mem.shape[1], mem.shape[0]),
        interpolation=cv2.INTER_AREA
    )
    results['pruned-stretched-lanczos'] = cv2.resize(
        results['pruned'],
        (mem.shape[1], mem.shape[0]),
        interpolation=cv2.INTER_LANCZOS4
    )

    return results


# Type for `lines_in_progress` (reused in several places):
_LIP_T: TypeAlias = OrderedDict[int, ImageLine]


@dataclass
class Image:
    FILE_PREFIX = FILE_PREFIX_DEFAULT

    # Ordered Map of image line numbers to the `ImageLine` objects built.
    # Using an OrderedDict instead of just a list b/c it will preserve both
    # received order and intended order and doesn't have to keep changing size
    # like a list would if totalLines isn't downlinked:
    lines_in_progress: _LIP_T

    # Guaranteed ULID (lexicographically sortable unique identifier) based on
    # GDS creation time:
    ulid_start: str = field(default_factory=lambda: str(ulid.new()))

    # Number of lines that were complete the last time a block was processed:
    lines_complete_on_last_block: int = 0

    # Dimensions of a full image (target size after correcting for
    # downsampling, etc.):
    full_image_columns: int = DEFAULT_IMAGE_WIDTH
    full_image_rows: int = DEFAULT_IMAGE_LINE_COUNT

    # Number of scanning sections in SBC flash:
    # (equivalent to n_interleave in Justin's algorithm):
    n_scan_sections: int = 8

    # Number of blank (i.e. vertical blanking interval) rows to ignore at the
    # top of the File Group (flash memory space):
    n_top_blank: int = 13  # 50  # 13

    # X-Index of the "Horizontal Seam line" (where the image should be wrapped
    # around):
    # This is the index of the last pixel that should be considered to be on
    # the left side of the seam (so, `0` if no seam.)
    idx_x_seam: int = 0  # 2018  # 2592

    # Y-Index of the "Vertical Seam line" (where the image should be wrapped
    # around):
    # This is the index of the last pixel that should be considered to be on
    # the top side of the seam (so `0` if no seam):
    idx_y_seam: int = 0

    def __str__(self) -> str:
        return (
            f"Image[{self.file_group_id}: "
            f"{len(self.lines_in_progress)} lines in progress, "
            f"{self.lines_complete_on_last_block} complete"
            "]"
        )

    @property
    def file_name_base(self):
        """Unique prefix to start all file names with.
        ULID before FGID so the names are lexicographically sortable:
        """
        return f"img_{self.FILE_PREFIX}__{self.ulid_start}__{self.file_group_id}"

    @property
    def num_lines_found(self) -> int:
        return len(self.lines_in_progress)

    @staticmethod
    @lru_cache(maxsize=3)
    def _total_lines_calculator(
        n_lines_in_progress: int,
        total_lines_vals: Tuple[int, ...]
    ) -> int:
        """Total number of lines in the `ImageLine` ("File Group").
        This is downlinked as part of the metadata block for each `ImageLine`
        ("File").

        Internal cached function to calculate the total number of lines (and
        check for consensus and sanity). Runs once per set of values.

        Args:
            n_lines_in_progress (int): Number of lines in progress
            total_lines_vals (Tuple[int]): `file_group_total_lines` from the
                metadata for every line in progress that has metadata.

        NOTE: This doesn't just take the OrderedDict[int, ImageLine] as an
        argument b/c `lru_cache` requires the arguments to be hashable and
        `OrderedDict` and `ImageLine` aren't (since they're mutable).
        Instead, all of the arguments are pre-chewed.
        """
        def _get_src(n_lines_in_progress: int, total_lines_vals: Tuple[int, ...]) -> int:
            """Internal helper function to find the appropriate source for the
            line count based on the data available."""
            # Return the default if no lines exist yet:
            if n_lines_in_progress == 0:
                return DEFAULT_IMAGE_LINE_COUNT

            # Check for consensus across all the file lines about the total line
            # count:
            # (ignore any sub-zero values):
            total_lines_vals = tuple(v for v in total_lines_vals if v > 0)
            if len(total_lines_vals) == 0:
                # Return the default if no valid estimates exist:
                app.logger.warning(
                    f"{n_lines_in_progress} lines have been received for an"
                    f"`Image` but no values for total line count. "
                    f"Either no metadata has been downlinked or all metadata line "
                    "total line count values are invalid (<=0)."
                )
                return DEFAULT_IMAGE_LINE_COUNT

            # Check for consensus:
            if any(v != total_lines_vals[0] for v in total_lines_vals):
                # Find most common value:
                hist = Counter(total_lines_vals)
                most_common = hist.most_common(1)[0][0]
                # Report the disagreement and resolution:
                app.logger.warning(
                    f"Disagreement about total line count. "
                    f"Counts of values reported in metadata are: "
                    f"`{Counter(total_lines_vals)}`. "
                    f"Using most common value of: `{most_common}`."
                )
                return most_common

            # If we get here, realistic values are present and they all agree.
            # So, use that agreed upon value:
            return total_lines_vals[0]

        total_lines = _get_src(n_lines_in_progress, total_lines_vals)

        # Make sure we haven't received more lines than this:
        if n_lines_in_progress > total_lines:
            app.logger.warning(
                f"There are {n_lines_in_progress} lines in `Image` "
                f"but `total_lines` was estimated to only be {total_lines}. "
                f"Perhaps there's been data corruption. "
                f"Using {n_lines_in_progress} as the total lines estimate "
                f"to prevent overflow."
            )
            total_lines = n_lines_in_progress

        return total_lines

    @property
    def total_lines(self) -> int:
        """Total number of lines in the `ImageLine` ("File Group").
        This is downlinked as part of the metadata block for each `ImageLine`
        ("File")."""
        return self._total_lines_calculator(
            n_lines_in_progress=len(self.lines_in_progress),
            total_lines_vals=tuple(
                line.metadata.file_group_total_lines
                for line in self.lines_in_progress.values()
                if line.metadata is not None
            )
        )

    @property
    def num_lines_complete(self) -> int:
        """Number of lines completed."""
        return sum(
            1 for line in self.lines_in_progress.values()
            if line.is_complete
        )

    @property
    def is_complete(self) -> bool:
        return (
            self.num_lines_found == self.total_lines
            and all(line.is_complete for line in self.lines_in_progress.values())
        )

    @property
    def file_group_id(self) -> int | None:
        """Hashed ID of this File Group (Image)."""
        if len(self.lines_in_progress) == 0:
            # No data yet.
            return None

        # Check for consensus:
        fg_ids = [l.file_group_id for l in self.lines_in_progress.values()]
        if any(fg_id != fg_ids[0] for fg_id in fg_ids):
            # Use most common value if disagreement:
            most_common = Counter(fg_ids).most_common(1)[0][0]
            return most_common
        else:
            return fg_ids[0]

    def validate_file_group_id(self) -> bool:
        """Checks if reported `file_group_id` is internally consistent within
        this file."""
        # Check for consensus:
        fg_ids = [l.file_group_id for l in self.lines_in_progress.values()]
        if any(fg_id != fg_ids[0] for fg_id in fg_ids):
            # Report the disagreement and resolution:
            app.logger.warning(
                f"Disagreement about `file_group_id` in `Image`. "
                f"Counts of values reported by lines in file are: "
                f"`{Counter(fg_ids)}`. "
                f"Using value: `{self.file_group_id = }`."
            )
            return False
        else:
            return True

    @property
    def callback_id(self) -> int | None:
        """Callback ID of this File Group (Image)."""
        if len(self.lines_in_progress) == 0:
            # No data yet.
            return None

        # Check for consensus among all lines with a metadata block:
        cids = [
            l.metadata.callback_id for l in self.lines_in_progress.values()
            if l.metadata is not None
        ]
        if any(cid != cids[0] for cid in cids):
            # Use most common value if disagreement:
            most_common = Counter(cids).most_common(1)[0][0]
            return most_common
        else:
            return cids[0]

    def validate_callback_id(self) -> bool:
        """Checks if reported `callback_id` is internally consistent within
        this file."""
        # Check for consensus:
        cids = [
            l.metadata.callback_id for l in self.lines_in_progress.values()
            if l.metadata is not None
        ]
        if any(cid != cids[0] for cid in cids):
            # Report the disagreement and resolution:
            app.logger.warning(
                f"Disagreement about `callback_id` in `Image`. "
                f"Counts of values reported by lines in file are: "
                f"`{Counter(cids)}`. "
                f"Using value: `{self.callback_id = }`."
            )
            return False
        else:
            return True

    @property
    def camera_num(self) -> int | None:
        """Camera Number of this File Group (Image)."""
        if len(self.lines_in_progress) == 0:
            # No data yet.
            return None

        # Check for consensus among all lines with a metadata block:
        cam_nums = [
            l.metadata.camera_num for l in self.lines_in_progress.values()
            if l.metadata is not None
        ]
        if any(cam_num != cam_nums[0] for cam_num in cam_nums):
            # Use most common value if disagreement:
            most_common = Counter(cam_nums).most_common(1)[0][0]
            return most_common
        else:
            return cam_nums[0]

    def validate_camera_num(self) -> bool:
        """Checks if reported `camera_num` is internally consistent within
        this file."""
        # Check for consensus:
        cam_nums = [
            l.metadata.camera_num for l in self.lines_in_progress.values()
            if l.metadata is not None
        ]
        if any(cam_num != cam_nums[0] for cam_num in cam_nums):
            # Report the disagreement and resolution:
            app.logger.warning(
                f"Disagreement about `camera_num` in `Image`. "
                f"Counts of values reported by lines in file are: "
                f"`{Counter(cam_nums)}`. "
                f"Using value: `{self.camera_num = }`."
            )
            return False
        else:
            return True

    def validate(self) -> bool:
        """Returns whether all lines in this file are valid & internally
        consistent."""
        # Collect as bools first so ALL validation checks are run
        # (this allows them to emit logs as needed)
        fg_id_valid = self.validate_file_group_id()
        callback_id_valid = self.validate_callback_id()
        cam_num_valid = self.validate_camera_num()
        all_lines_valid = all(
            line.validate()
            for line in self.lines_in_progress.values()
        )
        return (
            fg_id_valid and
            callback_id_valid and
            cam_num_valid and
            all_lines_valid
        )

    def fg_to_img_line_idx(self, fg_idx: int) -> int | None:
        """Returns index where a line of data should be placed in an image
        given it's file line index. Because of the scan order of the FPGA, the
        order of File Lines in File Group space don't match the order of the
        lines in the image, so this acts as transform between them.

        If a line shouldn't end up in the final image (i.e. all white line from
        the 8th sector), it's given an index of `None`.

        Based on the interleaving and skipping portion of Justin's algorithm.
        """
        scan_size = int(self.full_image_rows / self.n_scan_sections)
        scan_idx = fg_idx // scan_size

        # Ignore the last scan section (it's all white):
        # Ignore the blank section at the top (and any other odd pixels up
        # there):
        if (
            scan_idx == (self.n_scan_sections-1)
            or fg_idx < self.n_top_blank
        ):
            return None
        else:
            return (fg_idx % scan_size)*self.n_scan_sections + scan_idx

    def interim_build(
        self,
        pad_byte: bytes = b'\x00',  # Easier to see with 0x00 than 0xAA or 0xFF
        interpolate_unknown: bool = True,
        interactive_plot_result: bool = False,
        fg_idx_min: int | None = None,
        fg_idx_max: int | None = None
    ) -> np.ndarray:
        """Builds an interim version of the Image as a numpy array from interim
        data (a possibly incomplete dataset). This is designed to act as visual
        progress bar.
        Unknown cells will be filled with `pad_byte`.
        If `interpolate_unknown`, any cells where no valid data exists will be
        interpolated from adjacent cells (otherwise, just `pad_byte` will be
        used).

        If fg_idx of a line is outside of the inclusive bounds [fg_idx_min,
        fg_idx_max], it'll be ignored (useful for turning one full image of 7
        frames into 7 images).

        TODO: Figure out how to update this when we move to partial image
        downlinks (crops and sub/downsampling) with file slice summary logs.
        ^ - turns out this is actually just a way to perform downsampling and
        handles it implicitly if we only downlink any subset of the rows (so
        long as their line num is true to the original line num):
        """
        # Initialize memory space for full-size image:
        full_shape = (self.full_image_rows, self.full_image_columns)
        image: np.ma.MaskedArray = np.ma.MaskedArray(
            data=np.zeros(full_shape, dtype=np.uint8),
            mask=True
        )
        image_out: np.ndarray

        # Grab each line in File Group (memory) space and assign its value to
        # the appropriate row in the full-sized image space:
        # NOTE: The mask is automatically updated based on cells used.
        line_data: bytes
        for fg_idx, line in self.lines_in_progress.items():
            if fg_idx_min is not None and fg_idx < fg_idx_min:
                continue
            if fg_idx_max is not None and fg_idx > fg_idx_max:
                continue
            img_idx = self.fg_to_img_line_idx(fg_idx)
            if img_idx is None:
                # Skip. Don't add this line to the image (it's out-of-bounds):
                continue
            line_data = line.assemble()
            image[img_idx, 0:len(line_data)] = [int(x) for x in line_data]

        interpolation_failed: bool = False
        if interpolate_unknown:
            try:
                X, Y = np.mgrid[0:image.shape[0], 0:image.shape[1]]
                valid = ~image.mask  # has data
                interpolator = LinearNDInterpolator(
                    list(zip(X[valid], Y[valid])), image[valid]
                )
                image_out = interpolator(X, Y)
            except Exception as e:
                app.logger.debug(
                    f"Interim Image interpolation failed b/c `{e}`. "
                    f"Defaulting to fill with `{pad_byte[0]!r}`."
                )
                interpolation_failed = True

        # If interpolation fails or not successful, just use the pad byte as
        # fill:
        if not interpolate_unknown or interpolation_failed:
            image_out = image.filled(int(pad_byte[0]))

        # Apply seam fixes:
        image_out = np.roll(
            image_out, (-self.idx_y_seam, -self.idx_x_seam), axis=(0, 1)
        )

        if interactive_plot_result:
            import matplotlib.pyplot as plt  # type: ignore
            plt.figure()
            plt.imshow(image_out, cmap='gray')
            plt.show()

        return image_out

    def assemble(
        self,
        equalize_lengths: bool = True,
        pad_byte: bytes = b'\x00'
    ) -> List[bytes]:
        """Assembles all the lines in this file into a list of byte strings,
        one per line.
        If `equalize_lengths=True`, all lines will be padded with `pad_byte` to
        match the length of the longest line.
        """
        # Pre-allocate buffer for lines of appropriate size:
        lines_data: List[bytes] = [b''] * self.total_lines

        # Grab each line and assign its value to the appropriate row:
        for line_idx, line in self.lines_in_progress.items():
            lines_data[line_idx] = line.assemble()

        # Pad lines if needed:
        if equalize_lengths:
            # Make sure pad_byte is only 1B:
            if len(pad_byte) != 1:
                raise ValueError(
                    f"Attempting to pad array with `{pad_byte=!r}`. "
                    f"`pad_byte` needs to be exactly 1B long but instead is "
                    f"{len(pad_byte)}B long."
                )

            max_len = max(len(line_data) for line_data in lines_data)
            for i, line_data in enumerate(lines_data):
                size = len(line_data)
                diff = max_len - size
                if diff > 0:
                    app.logger.debug(
                        f"Adding {diff} pad bytes of `{pad_byte!r}` "
                        f"to line {i=}."
                    )
                    lines_data[i] = line_data + pad_byte*diff

        return lines_data

    def report(self) -> List[str]:
        """Returns a list of strings about the Image file status."""
        # Metadata completeness:
        num_lines_w_metadata = sum(
            1 for l in self.lines_in_progress.values()
            if l.metadata is not None
        )
        num_invalid_lines = sum(
            1 for l in self.lines_in_progress.values()
            if not l.validate()
        )
        num_corrupted_lines = sum(
            1 for l in self.lines_in_progress.values()
            if l.contains_corrupted_blocks()
        )

        return [
            f"File Group (Image) ID:\t {self.file_group_id}",
            f"Image Valid / Internally Consistent:\t {self.validate()}",
            f"Image Complete:\t {self.is_complete}",
            f"Total Image Lines:\t {self.total_lines}",
            f"Lines Found:\t {self.num_lines_found}",
            f"Lines w/ Metadata:\t {num_lines_w_metadata}",
            f"Lines Completed:\t {self.num_lines_complete}",
            f"Invalid / Internally Inconsistent Lines:\t {num_invalid_lines}",
            f"Lines with Corrupted Data:\t {num_corrupted_lines}",
        ]

    def _check_save_dir(self) -> str:
        # Make sure the save directory exists. Returns the save directory path.
        if not os.path.exists(OUTPUT_DIR_ROOT):
            os.makedirs(OUTPUT_DIR_ROOT)
        # Make the directory for all the files in this image:
        OUTPUT_DIR: Final = os.path.join(OUTPUT_DIR_ROOT, self.file_name_base)
        if not os.path.exists(OUTPUT_DIR):
            os.makedirs(OUTPUT_DIR)
        return OUTPUT_DIR

    def save_raw(self) -> None:
        """Most basic save operation to just save the raw data of this File and
        a pickle representation of this Image for later use."""
        app.logger.notice(f"Saving Raw {self.file_name_base} . . .")
        # Validate file first:
        if not self.validate():
            app.logger.warning(
                f"Saving file {self.file_name_base} but it's not valid and "
                f"likely contains corruption. "
                f"Other warnings should have been logged earlier "
                f"providing more details. "
                f"File: `{self}`"
            )

        # Log status report:
        full_report = (
            f"\tReport for Image {self.file_group_id}: \n\t\t"
            + ',\n\t\t'.join(self.report())
        )
        app.logger.info(full_report)

        # Get data for all lines:
        app.logger.info("Assembling Image . . .")
        lines_data: List[bytes] = \
            self.assemble(equalize_lengths=True, pad_byte=b'\xAA')

        # Build 2D integer array (split byte streams):
        app.logger.info("Building Integer Image . . .")
        lines_arr = [[int(x) for x in l] for l in lines_data]

        # Build raw file stream:
        app.logger.info("Building Raw File . . .")
        raw_file_data = b''.join(lines_data)

        # Build raw (greyscale, possibly bayered) image:
        app.logger.info("Building Raw Image . . .")
        raw_img = np.array(lines_arr, dtype=np.uint8)

        OUTPUT_DIR: Final = self._check_save_dir()

        # Save file report:
        report_file_name = self.file_name_base + ".report.txt"
        report_file_path = os.path.join(OUTPUT_DIR, report_file_name)
        app.logger.notice(
            f"\tSaving file report to {report_file_path} . . ."
        )
        with open(report_file_path, "w") as report_file:
            report_file.write(full_report + '\n')

        # Save a pickle of the entire Image object (an exact clone of this
        # object will allow us to retroactively investigate data that doesn't
        # make it into the final copy of the image, e.g. multiple copies of
        # image line blocks):
        pickle_file_name = self.file_name_base + ".image.pkl"
        pickle_file_path = os.path.join(OUTPUT_DIR, pickle_file_name)
        app.logger.notice(
            f"\tSaving Image pickle to {pickle_file_path} . . ."
        )
        with open(pickle_file_path, 'wb') as handle:
            pickle.dump(self, handle, protocol=pickle.HIGHEST_PROTOCOL)

        # Save raw file data:
        raw_file_name = self.file_name_base + ".raw"
        raw_file_path = os.path.join(OUTPUT_DIR, raw_file_name)
        app.logger.notice(
            f"\tSaving raw file to {raw_file_path} . . ."
        )
        with open(raw_file_path, "wb") as raw_file:
            # Write bytes to file
            raw_file.write(raw_file_data)

        # Save raw (greyscale, possibly bayered) image:
        raw_img_name = self.file_name_base + ".raw.png"
        raw_img_path = os.path.join(OUTPUT_DIR, raw_img_name)
        app.logger.notice(
            f"\tSaving {' x '.join(str(x) for x in raw_img.shape)} "
            f"raw (greyscale, possibly bayered) image "
            f"to {raw_img_path} . . ."
        )
        cv2.imwrite(raw_img_path, raw_img)

        app.logger.success(f"Saved {self.file_name_base}.")

    def save(self, interp: bool = True, debayer: bool = True) -> None:
        app.logger.notice(f"Saving {self.file_name_base} . . .")

        # Validate file first:
        if not self.validate():
            app.logger.warning(
                f"Saving file {self.file_name_base} but it's not valid and "
                f"likely contains corruption. "
                f"Other warnings should have been logged earlier "
                f"providing more details. "
                f"File: `{self}`"
            )

        # Log status report:
        full_report = (
            f"\tReport for Image {self.file_group_id}: \n\t\t"
            + ',\n\t\t'.join(self.report())
        )
        app.logger.info(full_report)

        # Get data for all lines:
        app.logger.info("Assembling Image . . .")
        lines_data: List[bytes] = \
            self.assemble(equalize_lengths=True, pad_byte=b'\xAA')

        # Log some metadata:
        n_rows = len(lines_data)
        n_cols_max = max(len(line) for line in lines_data)
        app.logger.info(
            f"File {self.file_name_base} is {n_rows}R x {n_cols_max}C."
        )

        # Build raw file stream:
        app.logger.info("Building Raw File . . .")
        raw_file_data = b''.join(lines_data)

        # Build 2D integer array (split byte streams):
        app.logger.info("Building Integer Image . . .")
        lines_arr = [[int(x) for x in l] for l in lines_data]

        # Build text file version of file where each line is on a new line:
        app.logger.info("Building Raw Image Text File (long) . . .")
        txt_file_data = [
            # Group every 4B (U32) for sequence counting:
            ' : '.join(
                ':'.join(f'{x:02X}' for x in sl)
                for sl in zip_longest(*([iter(l)]*4))
            )
            for l in lines_arr
        ]
        # Append ImageLine summary string to each line:
        line_summaries: List[ImageLine | None] = [None] * len(txt_file_data)
        # ... fill in all lines present:
        for line_idx, line in self.lines_in_progress.items():
            line_summaries[line_idx] = line
        # ... prefix all the lines:
        for i, line_summary in enumerate(line_summaries):
            txt_file_data[i] = (
                str(line_summary).expandtabs(4).ljust(250, ' ')
                + txt_file_data[i]
            )

        # Building image:
        if interp:
            app.logger.info("Building Interpolated Image (very long) . . .")
            interp_image = self.interim_build(interpolate_unknown=True)
        else:
            app.logger.info("Skipping Interpolation. Building padded image...")
            interp_image = self.interim_build(interpolate_unknown=False)

        # Build raw (greyscale, possibly bayered) image:
        app.logger.info("Building Raw Image . . .")
        raw_img = np.array(lines_arr, dtype=np.uint8)

        # Debayer image:
        if debayer:
            app.logger.info("Building Debayered Color Image . . .")
            color_img = cv2.cvtColor(raw_img, cv2.COLOR_BAYER_BG2BGR)

        OUTPUT_DIR: Final = self._check_save_dir()

        # Save file report:
        report_file_name = self.file_name_base + ".report.txt"
        report_file_path = os.path.join(OUTPUT_DIR, report_file_name)
        app.logger.notice(
            f"\tSaving file report to {report_file_path} . . ."
        )
        with open(report_file_path, "w") as report_file:
            report_file.write(full_report + '\n')

        # Save a pickle of the entire Image object (an exact clone of this
        # object will allow us to retroactively investigate data that doesn't
        # make it into the final copy of the image, e.g. multiple copies of
        # image line blocks):
        pickle_file_name = self.file_name_base + ".image.pkl"
        pickle_file_path = os.path.join(OUTPUT_DIR, pickle_file_name)
        app.logger.notice(
            f"\tSaving Image pickle to {pickle_file_path} . . ."
        )
        with open(pickle_file_path, 'wb') as handle:
            pickle.dump(self, handle, protocol=pickle.HIGHEST_PROTOCOL)

        # Save interpolated image:
        interp_file_name = self.file_name_base + ".interp.png"
        interp_file_path = os.path.join(OUTPUT_DIR, interp_file_name)
        app.logger.notice(
            f"\tSaving interpolated file to {interp_file_path} . . ."
        )
        cv2.imwrite(interp_file_path, interp_image)

        # Save raw file data:
        raw_file_name = self.file_name_base + ".raw"
        raw_file_path = os.path.join(OUTPUT_DIR, raw_file_name)
        app.logger.notice(
            f"\tSaving raw file to {raw_file_path} . . ."
        )
        with open(raw_file_path, "wb") as raw_file:
            # Write bytes to file
            raw_file.write(raw_file_data)

        # Save raw file data as text:
        txt_file_name = self.file_name_base + ".raw.txt"
        txt_file_path = os.path.join(OUTPUT_DIR, txt_file_name)
        app.logger.notice(
            f"\tSaving raw as text file to {txt_file_path} . . ."
        )
        with open(txt_file_path, "w") as txt_file:
            # Write bytes to file
            txt_file.write('\n'.join(txt_file_data) + '\n')

        # Save raw (greyscale, possibly bayered) image:
        raw_img_name = self.file_name_base + ".raw.png"
        raw_img_path = os.path.join(OUTPUT_DIR, raw_img_name)
        app.logger.notice(
            f"\tSaving {' x '.join(str(x) for x in raw_img.shape)} "
            f"raw (greyscale, possibly bayered) image "
            f"to {raw_img_path} . . ."
        )
        cv2.imwrite(raw_img_path, raw_img)

        # Save debayered (possibly color) image:
        if debayer:
            color_img_name = self.file_name_base + ".color.png"
            color_img_path = os.path.join(OUTPUT_DIR, color_img_name)
            app.logger.notice(
                f"\tSaving {' x '.join(str(x) for x in color_img.shape)} "
                f"color (debayered) image "
                f"to {color_img_path} . . ."
            )
            cv2.imwrite(color_img_path, color_img)

        # Apply and Save Justin's Algorithm:
        app.logger.notice("Applying Reconstruction Algorithm . . .")
        app.logger.info("Applying Justin's Algorithm . . .")
        j_results = justin_algorithm(raw_img)
        for res_name, res_img in j_results.items():
            app.logger.info(f"\t> Debayering {res_name} JA result . . .")
            j_images = [res_img]
            if debayer:
                # Apply all possible debayering patterns to each result:
                patterns = OrderedDict([
                    ('BG2RGB', cv2.COLOR_BAYER_BG2RGB),  # RGGB (likely)
                    ('BG2BGR', cv2.COLOR_BAYER_BG2BGR),  # RGGB (likely)
                    ('GB2RGB', cv2.COLOR_BAYER_GB2RGB),  # GRBG (likely)
                    ('GB2BGR', cv2.COLOR_BAYER_GB2BGR),  # GRBG (likely)
                    ('RG2RGB', cv2.COLOR_BAYER_RG2RGB),  # BGGR (unlikely)
                    ('GR2RGB', cv2.COLOR_BAYER_GR2RGB)  # GBRG (unlikely)
                ])
                for pattern in patterns.values():
                    j_images.append(cv2.cvtColor(res_img, pattern))
            else:
                patterns = OrderedDict()
            # Save each version of this result:
            app.logger.info(f"\t> Saving all {res_name} JA images . . .")
            for i, j_img in enumerate(j_images):
                pattern_name = [*patterns.keys()][i-1] if i > 0 else '_none'
                img_name = (
                    f"{self.file_name_base}__justin_{res_name}__p{i}"
                    f"_{pattern_name}.png"
                )
                img_path = os.path.join(OUTPUT_DIR, img_name)
                app.logger.notice(
                    f"\tSaving Justin's Algorithm Output "
                    f"(raw image -> Justin's Algorithm -> {pattern_name}) "
                    f"to {img_path} . . ."
                )
                cv2.imwrite(img_path, j_img)

        app.logger.success(f"Saved {self.file_name_base}.")

    def add_block(self, block: FileBlockPayload) -> None:
        """Adds the given `FileBlockPayload` to the appropriate line in this
        file.
        """
        line_id = block.file_group_line_number
        if line_id not in self.lines_in_progress:
            self.lines_in_progress[line_id] = ImageLine.from_first_block(block)
        else:
            self.lines_in_progress[line_id].add_block(block)

        lines_complete = self.num_lines_complete

        # Log line completion only as it increases:
        if lines_complete > self.lines_complete_on_last_block:
            app.logger.info(f"\t{lines_complete} Lines Complete . . .")

        self.lines_complete_on_last_block = lines_complete

    @classmethod
    def new_blank(cls: Type[Image]) -> Image:
        """Initializes a new blank Image."""
        return cls(
            lines_in_progress=OrderedDict()
        )


@dataclass
class BasicImageDecoder:
    FILE_PREFIX = FILE_PREFIX_DEFAULT
    images_in_progress: Dict[FileIdType, Image]

    @classmethod
    def new_empty(cls: Type[BasicImageDecoder]) -> BasicImageDecoder:
        """Creates a new empty `BasicImageDecoder`."""
        return cls(dict())

    def finish_image(self, file_group_id: int) -> None:
        """Routine to perform when done with an image."""
        # TODO: Emit file
        # Once file is emitted, delete it from the in_progress list
        # since hashed IDs aren't guaranteed to be unique across the
        # duration of the entire mission since they're derived from a
        # timestamp, which will overflow and will have collisions every
        # 2^16 IDs at best:
        del self.images_in_progress[file_group_id]

    def process_file_block(self, block: FileBlockPayload) -> None:
        file_group_id = block.file_group_id
        if file_group_id not in self.images_in_progress:
            self.images_in_progress[file_group_id] = Image.new_blank()
            self.images_in_progress[file_group_id].FILE_PREFIX = self.FILE_PREFIX

        image = self.images_in_progress[file_group_id]

        image.add_block(block)

        if image.is_complete:
            app.logger.success(
                f"IMAGE COMPLETE: `{file_group_id}`.\n"
            )
            image.save()
            self.finish_image(file_group_id)

    def export_all_raw(self):
        """Forces an export of just raw data for all current in-process images."""
        for image in self.images_in_progress.values():
            image.save_raw()

    def export_all(self, interp: bool = True, debayer: bool = True):
        """Forces an export of all current in-process images."""
        for image in self.images_in_progress.values():
            image.save(debayer=debayer, interp=interp)

    def process_file_blocks_in_packet(self, packet: Packet) -> None:
        for block in packet.payloads[FileBlockPayload]:
            block = cast(FileBlockPayload, block)
            self.process_file_block(block)


def main(opts) -> None:
    decoder = BasicImageDecoder.new_empty()
    while True:
        try:
            ipc_payload = manager.read('sub')
            try:
                msg = ipc.guard_msg(ipc_payload.message,
                                    DownlinkedPayloadsMessage)
                payloads = msg.content.payloads
            except Exception as e:
                app.logger.error(
                    f"Failed to decode IPC message `{msg}` "
                    f"of `{ipc_payload=}` b/c: `{e}`."
                )
            for block in payloads[FileBlockPayload]:
                block = cast(FileBlockPayload, block)
                decoder.process_file_block(block)
        except KeyboardInterrupt as ki:
            """User can press key to export all in-progress images."""
            decoder.export_all(
                interp=opts.interp,
                debayer=opts.debayer
            )


parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — Image Builder Demo — CLI'
))


def get_opts():
    parser.add_argument('--interp', default=True,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            "Whether or not to attempt to interpolate unknown pixels."
                        ))
    parser.add_argument('--debayer', default=False,
                        action=argparse.BooleanOptionalAction,
                        help=(
                            "Whether or not to attempt debayering of images."
                        ))
    return parser.parse_args()


if __name__ == "__main__":
    opts = get_opts()
    main(opts)
