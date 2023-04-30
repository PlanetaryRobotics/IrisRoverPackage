"""
Starting implementation of an IPC file builder.

Run the Transceiver TopicProxy in a separate terminal before running this
script and keep it running for the duration of this script:
```
make proxies
```

Also run a Transceiver to supply packets (e.g. PcapTransceiver).

@author: Connor W. Colombo (CMU)
@last-updated: 04/29/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

import os
import cv2
import ulid
import os.path
from functools import lru_cache
import dataclasses
from dataclasses import dataclass
from itertools import zip_longest
from collections import OrderedDict, Counter
from typing import Dict, List, Tuple, cast, TypeAlias, Type, Final, Callable

import numpy as np

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket
from IrisBackendv3.codec.payload import FileBlockPayload, FileMetadata
from IrisBackendv3.ipc.messages import (
    DownlinkedPayloadsMessage
)
IB3.init_from_latest()

app = ipc.IpcAppHelper("FileBuilderDemo")

# Where output files get saved (w.r.t GroundSoftware)
OUTPUT_DIR: Final[str] = './out/image_file_decoding_tests/'

# Prefix to apply to all files:
FILE_PREFIX: Final[str] = 'RC9_Test_Img'

# Default number of lines in an image:
DEFAULT_IMAGE_LINE_COUNT: Final[int] = 1944

FileIdType: TypeAlias = int
CallbackIdType: TypeAlias = int

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

    @property
    def num_blocks_found(self) -> int:
        return len(self.data_block_payloads)

    @property
    def total_num_blocks(self) -> int:
        return len(self.ordered_data_blocks)

    @property
    def is_complete(self) -> bool:
        return self.num_blocks_found == self.total_num_blocks

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
        # Check for consensus:
        b0_num = self.data_block_payloads[0].file_group_line_number
        if any(b.file_group_line_number != b0_num for b in self.data_block_payloads):
            # Report the disagreement and resolution:
            nums = [b.file_group_line_number for b in self.data_block_payloads]
            app.logger.warning(
                f"For `ImageLine` with metadata: `{self.metadata}`: "
                f"Disagreement about `file_group_line_number`. "
                f"Values reported by blocks in file line are: `{nums}`. "
                f"Using value: `{self.line_num = }`."
            )
            return False
        else:
            return True

    def validate_file_group_id(self) -> bool:
        """Checks if reported `file_group_id` is internally consistent within
        this line."""
        # Check for consensus:
        b0_fg_id = self.data_block_payloads[0].file_group_id
        if any(b.file_group_id != b0_fg_id for b in self.data_block_payloads):
            # Report the disagreement and resolution:
            fg_ids = [b.file_group_id for b in self.data_block_payloads]
            app.logger.warning(
                f"For `ImageLine` with `line_num = {self.line_num}` "
                f"and metadata: `{self.metadata}`: "
                f"Disagreement about `file_group_id`. "
                f"Values reported by blocks in file line are: `{fg_ids}`. "
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
                f"Space allocated for blocks: `{self.num_blocks=}`. "
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
            self.metadata is None and
            len(self.data_block_payloads) == 0 and
            len(self.ordered_data_blocks) == 0
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
        found first (should be `FileMetadata` but that's not guaranteed)."""
        line = cls.new_empty()
        # `add_block` automatically handles sizing the line, so we don't have
        # to worry about it here...
        line.add_block(block)
        if not block.is_metadata:
            app.logger.warning(
                f"First block for `ImageLine` with "
                f"line={block.file_group_line_number} in {block.file_group_id} "
                f"was not `FileMetadata`. This is unusual but not impossible."
            )
        return line


# Type for `lines_in_progress` (reused in several places):
_LIP_T: TypeAlias = OrderedDict[int, ImageLine]


@dataclass
class Image:
    # Ordered Map of image line numbers to the `ImageLine` objects built.
    # Using an OrderedDict instead of just a list b/c it will preserve both
    # received order and intended order and doesn't have to keep changing size
    # like a list would if totalLines isn't downlinked:
    lines_in_progress: _LIP_T

    # Number of lines that were complete the last time a block was processed:
    lines_complete_on_last_block: int = 0

    @property
    def num_lines_found(self) -> int:
        return len(self.lines_in_progress)

    @staticmethod
    @lru_cache(maxsize=3)
    def _total_lines_calculator(lines_in_progress: _LIP_T) -> int:
        """Total number of lines in the `ImageLine` ("File Group").
        This is downlinked as part of the metadata block for each `ImageLine`
        ("File").

        Internal cached function to calculate the total number of lines (and
        check for consensus and sanity). Runs once per set of values.
        """
        # Internal helper function to find the appropriate source for the line
        # count based on the data available:
        def _get_src(lines_in_progress: _LIP_T) -> int:
            # Return the default if no lines exist yet:
            if len(lines_in_progress) == 0:
                return DEFAULT_IMAGE_LINE_COUNT

            # Check for consensus across all the file lines about the total line
            # count:
            total_lines_vals = [
                line.metadata.file_group_total_lines
                for line in lines_in_progress.values()
                if line.metadata is not None
            ]
            total_lines_vals = [v for v in total_lines_vals if v > 0]
            if len(total_lines_vals) == 0:
                # Return the default if no valid estimates exist:
                app.logger.warning(
                    f"{len(lines_in_progress)} lines have been received for an"
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
                    f"Values reported in metadata are: `{total_lines_vals}`. "
                    f"Using most common value of: `{most_common}`."
                )
                return most_common

            # If we get here, realistic values are present and they all agree.
            # So, use that agreed upon value:
            return total_lines_vals[0]

        total_lines = _get_src(lines_in_progress)

        # Make sure we haven't received more lines than this:
        if len(lines_in_progress) > total_lines:
            app.logger.warning(
                f"There are {len(lines_in_progress)} lines in `Image` "
                f"but `total_lines` was estimated to only be {total_lines}. "
                f"Perhaps there's been data corruption. "
                f"Using {len(lines_in_progress)} as the total lines estimate "
                f"to prevent overflow."
            )
            total_lines = lines_in_progress

        return total_lines

    @property
    def total_lines(self) -> int:
        """Total number of lines in the `ImageLine` ("File Group").
        This is downlinked as part of the metadata block for each `ImageLine`
        ("File")."""
        return self._total_lines_calculator(self.lines_in_progress)

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
            # Use most common value:
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
                f"Values reported by lines in file are: `{fg_ids}`. "
                f"Using value: `{self.file_group_id = }`."
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
        all_lines_valid = all(
            line.validate()
            for line in self.lines_in_progress.values()
        )
        return (
            fg_id_valid and
            all_lines_valid
        )

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

    def save(self) -> None:
        # Make file name base for all files:
        file_name_base = f"img_{FILE_PREFIX}__{ulid.new()}__{self.file_group_id}"

        app.logger.notice(f"Saving {file_name_base} . . .")

        # Validate file first:
        if not self.validate():
            app.logger.warning(
                f"Saving file {file_name_base} but it's not valid and likely "
                f"contains corruption. "
                f"Other warnings should have been logged earlier "
                f"providing more details. "
                f"File: `{self}`"
            )

        # Log status report:
        app.logger.info(
            f"\tReport \n\t\t"
            + ',\n\t\t'.join(self.report())
        )

        # Get data for all lines:
        lines_data: List[bytes] = \
            self.assemble(equalize_lengths=True, pad_byte=b'\xAA')

        # Log some metadata:
        n_rows = len(lines_data)
        n_cols_max = max(len(line) for line in lines_data)
        app.logger.info(
            f"File {file_name_base} is {n_rows} x {n_cols_max}."
        )

        # Build raw file stream:
        raw_file_data = b''.join(lines_data)

        # Build 2D integer array (split byte streams):
        lines_arr = [[int(x) for x in l] for l in lines_data]

        # Build text file version of file where each line is on a new line:
        txt_file_data = [
            # Group every 4B (U32) for sequence counting:
            ' : '.join(
                ':'.join(f'{x:02X}' for x in sl)
                for sl in zip_longest(*([iter(l)]*4))
            )
            for l in lines_arr
        ]

        # Build raw (greyscale, possibly bayered) image:
        raw_img = np.array(lines_arr)

        # Debayer image:
        color_img = cv2.cvtColor(raw_img, cv2.COLOR_BAYER_BG2BGR)

        # Make sure the save directory exists:
        if not os.path.exists(OUTPUT_DIR):
            os.makedirs(OUTPUT_DIR)

        # Save raw file data:
        raw_file_name = file_name_base + ".raw"
        raw_file_path = os.path.join(OUTPUT_DIR, raw_file_name)
        app.logger.notice(
            f"\tSaving raw file to {raw_file_path} . . ."
        )
        with open(raw_file_path, "wb") as raw_file:
            # Write bytes to file
            raw_file.write(raw_file_data)

        # Save raw file data as text:
        txt_file_name = file_name_base + ".raw.txt"
        txt_file_path = os.path.join(OUTPUT_DIR, txt_file_name)
        app.logger.notice(
            f"\tSaving raw as text file to {txt_file_path} . . ."
        )
        with open(txt_file_path, "w") as txt_file:
            # Write bytes to file
            txt_file.writelines(txt_file_data)

        # Save raw (greyscale, possibly bayered) image:
        raw_img_name = file_name_base + ".raw.png"
        raw_img_path = os.path.join(OUTPUT_DIR, raw_img_name)
        app.logger.notice(
            f"\tSaving {' x '.join(str(x) for x in raw_img.shape)} "
            f"raw (greyscale, possibly bayered) image "
            f"to {raw_img_path} . . ."
        )
        cv2.imwrite(raw_img_path, raw_img)

        # Save raw (greyscale, possibly bayered) image:
        color_img_name = file_name_base + ".color.png"
        color_img_path = os.path.join(OUTPUT_DIR, color_img_name)
        app.logger.notice(
            f"\tSaving {' x '.join(str(x) for x in color_img.shape)} "
            f"color (debayered) image "
            f"to {color_img_path} . . ."
        )
        cv2.imwrite(color_img_path, color_img)

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

        image = self.images_in_progress[file_group_id]

        image.add_block(block)

        if image.is_complete:
            app.logger.success(
                f"IMAGE COMPLETE: `{file_group_id}`.\n"
            )
            image.save()
            self.finish_image(file_group_id)

    def export_all(self):
        """Forces an export of all current in-process images."""
        for image in self.images_in_progress.values():
            image.save()

    def process_file_blocks_in_packet(self, packet: Packet) -> None:
        blocks = packet.payloads[FileBlockPayload]
        for block in blocks:
            self.process_file_block(block)


def main() -> None:
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
            for block in payloads[IB3.codec.payload.FileBlockPayload]:
                decoder.process_file_block(block)
        except KeyboardInterrupt as ki:
            """User can press key to export all in-progress images."""
            decoder.export_all()


if __name__ == "__main__":
    main()
