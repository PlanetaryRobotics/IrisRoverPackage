"""
Really crude starting implementation of an IPC file builder.

@author: Connor W. Colombo (CMU)
@last-updated: 03/11/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

import os
import cv2
import ulid
import os.path
from dataclasses import dataclass
from collections import OrderedDict
from typing import Dict, List, Tuple, cast, TypeAlias, Type, Final

import numpy as np

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.codec.packet import Packet, IrisCommonPacket
from IrisBackendv3.codec.payload import FileBlockPayload, FileMetadata
from IrisBackendv3.ipc.messages import (
    DownlinkedPacketsMessage
)
IB3.init_from_latest()

app = ipc.IpcAppHelper("FileBuilderDemo")

# For now, files have fixed length. This:
FIXED_FILE_LINES: Final[int] = 500

# Where output files get saved (w.r.t GroundSoftware)
OUTPUT_DIR: Final[str] = './out/image_file_decoding_tests/'

# Prefix to apply to all files:
FILE_PREFIX: Final[str] = '500x500_1'


HashedIdType: TypeAlias = int
FileLineIdType: TypeAlias = HashedIdType
FileIdType: TypeAlias = HashedIdType
CallbackIdType: TypeAlias = int

# Setup:
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER,
        topics=[ipc.Topic.DL_PACKETS],
        bind=False
    )
})


@dataclass
class FileLine:
    # Metadata for this file line:
    metadata: FileMetadata | None
    # Growing list of all file block payloads found, **in the order found**:
    data_block_payloads: List[FileBlockPayload]
    # Fixed list of all file block payloads in this file (length stays fixed at
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
                f"For file line with metadata: `{self.metadata}`."
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
                f"File line with metadata: `{self.metadata}` "
                f"contains the following possibly corrupted lines: "
                f"{possibly_corrupted_lines}."
            )
        return len(possibly_corrupted_lines) > 0

    def validate(self) -> bool:
        """Returns whether this file line is valid & internally consistent."""
        valid = True
        valid &= self.validate_total_blocks()
        valid &= (not self.contains_corrupted_blocks())
        return valid

    def assemble(self) -> bytes:
        """Assembles all the blocks in this file line into one byte string
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
        """Adds the given `FileBlockPayload` to this file line.
        Adjusts `ordered_data_blocks` if necessary (only necessary if
        `total_blocks` was corrupted in a previous block).
        """
        block_idx = self.block_num_to_idx(block.block_number)
        total_blocks = block.total_blocks
        # Determine what size this block thinks this line should be (make a
        # best guess in case it's corrupted):
        target_size = max(total_blocks, block_idx+1)

        # Shrink `total_blocks` if necessary (and possible):
        if target_size < self.total_num_blocks:
            # Only allow shrinking if no data has been written past the
            # threshold proposed by this block:
            can_shrink = all(
                d == b'' for d in self.ordered_data_blocks[target_size:]
            )
            if can_shrink:
                self.ordered_data_blocks = self.ordered_data_blocks[target_size:]

        # Grow `total_blocks` if necessary:
        if target_size > self.total_num_blocks:
            diff = target_size - self.total_num_blocks
            self.ordered_data_blocks += [b'']*diff

        # If this block is metadata, just handle it as metadata:
        if block.is_metadata:
            self.metadata = block
        else:
            # This is a data block:
            # Append this block to the list of blocks collected:
            self.data_block_payloads.append(block)
            # Add this block's data to the right part of the line:
            self.ordered_data_blocks[block_idx] = block.data

    @classmethod
    def from_first_block(cls: Type[FileLine], block: FileBlockPayload) -> FileLine:
        """Initializes this `FileLine` from whatever `FileBlockPayload` was
        found first (should be `FileMetadata` but that's not guaranteed)."""
        line = cls(
            metadata=None,
            data_block_payloads=[],
            ordered_data_blocks=[]
        )
        # `add_block` automatically handles sizing the line, so we don't have
        # to worry about it here...
        line.add_block(block)
        if not block.is_metadata:
            app.logger.warning(
                f"First block for FileLine with hashedId={block.hashed_id} "
                f"was not `FileMetadata`. This is unusual but not impossible."
            )
        return line


@dataclass
class File:
    total_lines: int
    # TODO: Currently lines are just handled in the order they're found.
    # We should add explicit ordering to the block metadata and handle lines
    # the same way lines handle blocks.
    lines_in_progress: OrderedDict[FileLineIdType, FileLine]

    # Number of lines that were complete the last time a block was processed:
    lines_complete_on_last_block: int = 0

    @property
    def num_lines_found(self) -> int:
        return len(self.lines_in_progress)

    @property
    def is_complete(self) -> bool:
        return (
            self.num_lines_found == self.total_lines
            and all(line.is_complete for line in self.lines_in_progress.values())
        )

    def validate(self) -> bool:
        """Returns whether all lines in this file are valid & internally
        consistent."""
        all_lines_valid = all(
            line.validate()
            for line in self.lines_in_progress.values()
        )
        return all_lines_valid

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
        lines_data = [
            line.assemble()
            for line in self.lines_in_progress.values()
        ]
        if equalize_lengths:
            max_len = max(len(line_data) for line_data in lines_data)
            for i, line_data in enumerate(lines_data):
                size = len(line_data)
                diff = max_len - size
                if diff > 0:
                    lines_data[i] = line_data + pad_byte*diff
        return lines_data

    def save(self) -> None:
        # Make file name base for all files:
        file_name_base = f"img_{FILE_PREFIX}__{ulid.new()}"

        # Validate file first:
        if not self.validate():
            app.logger.warning(
                f"Saving file {file_name_base} but it's not valid and likely "
                f"contains corruption. "
                f"Other warnings should have been logged earlier "
                f"providing more details. "
                f"File: `{self}`"
            )

        # Get data for all lines:
        lines_data = self.assemble(equalize_lengths=True, pad_byte=b'\xAA')

        # Log some metadata:
        n_rows = len(lines_data)
        n_cols_max = max(len(line) for line in lines_data)
        app.logger.info(
            f"File {file_name_base} is {n_rows} x {n_cols_max}."
        )

        # Build raw file stream:
        raw_file_data = b''.join(lines_data)

        # Build raw (greyscale, possibly bayered) image:
        raw_img = np.asarray(bytearray(b''.join(lines_data)))
        raw_img = raw_img.reshape((-1, n_cols_max))

        # Debayer image:
        color_img = cv2.cvtColor(raw_img, cv2.COLOR_BAYER_BG2BGR)

        # Make sure the save directory exists:
        if not os.path.exists(OUTPUT_DIR):
            os.makedirs(OUTPUT_DIR)

        # Save raw file data:
        raw_file_name = file_name_base + ".raw"
        raw_file_path = os.path.join(OUTPUT_DIR, raw_file_name)
        app.logger.notice(
            f"Saving raw file to {raw_file_path} . . ."
        )
        with open(raw_file_path, "wb") as raw_file:
            # Write bytes to file
            raw_file.write(raw_file_data)

        # Save raw (greyscale, possibly bayered) image:
        raw_img_name = file_name_base + ".raw.png"
        raw_img_path = os.path.join(OUTPUT_DIR, raw_img_name)
        app.logger.notice(
            f"Saving raw (greyscale, possibly bayered) image "
            f"to {raw_img_path} . . ."
        )
        cv2.imwrite(raw_img_path, raw_img)

        # Save raw (greyscale, possibly bayered) image:
        color_img_name = file_name_base + ".color.png"
        color_img_path = os.path.join(OUTPUT_DIR, color_img_name)
        app.logger.notice(
            f"Saving color (debayered) image "
            f"to {color_img_path} . . ."
        )
        cv2.imwrite(color_img_path, color_img)

    def add_block(self, block: FileBlockPayload) -> None:
        """Adds the given `FileBlockPayload` to the appropriate line in this
        file.
        """
        line_id = block.hashed_id
        if line_id not in self.lines_in_progress:
            self.lines_in_progress[line_id] = FileLine.from_first_block(block)
        else:
            self.lines_in_progress[line_id].add_block(block)

        lines_complete = sum(
            line.is_complete
            for line in self.lines_in_progress.values()
        )

        if lines_complete > self.lines_complete_on_last_block:
            app.logger.info(f"\t{lines_complete} Lines Complete . . .")

        self.lines_complete_on_last_block = lines_complete

    @classmethod
    def new_blank(cls: Type[File], total_lines: int) -> File:
        """Initializes a new blank file with the given fixed number of lines."""
        return cls(
            lines_in_progress=OrderedDict(),
            total_lines=total_lines
        )


@dataclass
class BasicImageDecoder:
    # If we haven't received the metadata block for a file, we don't know what
    # it's ID is yet, so we'll just collect its blocks under the `None` ID:
    files_in_progress: Dict[FileIdType | None, File]

    @classmethod
    def new_empty(cls: Type[BasicImageDecoder]) -> BasicImageDecoder:
        """Creates a new empty `BasicImageDecoder`."""
        return cls(dict())

    def process_file_block(self, block: FileBlockPayload) -> None:
        # TODO: Current FSW doesn't support building multiple files at once
        # since lines aren't ID'd to files. For now, we'll just build one file
        # at a time under the `None` ID.
        if None not in self.files_in_progress:
            self.files_in_progress[None] = File.new_blank(
                total_lines=FIXED_FILE_LINES
            )

        file = self.files_in_progress[None]

        file.add_block(block)

        if file.is_complete:
            app.logger.success(
                f"FILE COMPLETE: `{file=}`.\n"
            )
            file.save()
            # TODO: Once file is emitted, delete it from the in_progress list
            # # (since hashIds aren't guaranteed to be unique across the
            # duration of the entire mission -- derived from a timestamp, which
            # will overflow)

    def export_all(self):
        """Forces an export of all current in-process images."""
        for file in self.files_in_progress.values():
            file.save()

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
                                    DownlinkedPacketsMessage)
                packets = msg.content.packets
            except Exception as e:
                app.logger.error(
                    f"Failed to decode IPC message `{msg}` "
                    f"of `{ipc_payload=}` b/c: `{e}`."
                )
            for packet in packets:
                decoder.process_file_blocks_in_packet(packet)
        except KeyboardInterrupt as ki:
            """User can press key to export all in-progress images."""
            decoder.export_all()


if __name__ == "__main__":
    main()
