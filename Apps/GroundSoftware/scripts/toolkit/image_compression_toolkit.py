"""
Toolkit for compressing & decompressing images both losslessly,
using heatshrink, and lossy using a bayer-preserving binning procedure,
similar to how the Aptina sensor does it, but in a way that preserves
RC11-style metadata.

@author: Connor W. Colombo (CMU)
@last-updated: 10/16/2023
"""

import heatshrink2 as hs2
from typing import Final, Tuple, List, Dict


# Default binning amount:
N_BIN_DEFAULT: Final[int] = 7
# Heatshrink settings, shown by heatshrink test sandbox to be optimal values
# for our images:
HS_WINDOW: Final[int] = 10
HS_LOOKAHEAD: Final[int] = 3


LEN_OF_LINE_HEADER = 15  # Bytes
LEN_OF_FRAME_HEADER_DATA = 84  # Bytes


def compress(line: bytes, n_bin: int = N_BIN_DEFAULT, heatshrink: bool = True) -> bytes:
    # Bin and heatshrink. Also RLE homogeneous lines.
    # Impl. in a way that's easily translatable to C++
    line = bytearray(line)  # make it mutable
    line_len: int = len(line)
    if (line_is_homogeneous(line) and len(line) > 6):
        # If Line is all 1 value, replace with:
        # b"ALL" + (U16 num bytes) + char to repeat (really basic RLE)
        line[0:3] = b"ALL"
        line[3] = line_len & 0xFF

        line[4] = (line_len >> 8) & 0xFF
        line[5] = line[5]
        line_len = 6
        line = line[:line_len]
    else:
        # Bin Line:
        line, line_len = bin_line(n_bin, line)
        line = line[:line_len]

        # Heatshrink Line (heatshrink test sandbox showed these settings to be
        # optimal values for our images):
        if heatshrink:
            compressed = hs2.compress(
                line, window_sz2=HS_WINDOW, lookahead_sz2=HS_LOOKAHEAD)
            compressed_len = len(compressed)

        # Only downlink compressed version if compression helped:
        if heatshrink and compressed_len < line_len:
            line = b'\xFF' + compressed  # flag as compressed
            line_len = compressed_len + 1
        else:
            line = b'\x00' + line  # flag as not compressed
            line_len = line_len + 1

    return bytes(line[:line_len])


def decompress(line: bytes) -> bytes:
    # Unheatshrink & unbin. Handle RLE for homogeneous lines.
    # Doesn't need to be easily translatable to C++ since this will always be
    # run on GSW.
    line = bytearray(line)  # make it mutable
    if line[:3] == b"ALL" and len(line) == 6:
        # A homogeneous line that was RLE'd:
        line_len = (line[4] << 8) | line[3]
        line = line[5:6] * line_len
    else:
        # Decompress (if necessary):
        if (line[0] == 0xFF):
            line = hs2.decompress(
                line[1:], window_sz2=HS_WINDOW, lookahead_sz2=HS_LOOKAHEAD)
        else:
            line = line[1:]
        # Unbin:
        line = unbin_line(line)
    return bytes(line)


# Use Aptina-style (bayer-preserving) binning as a method of acceptably lossy compression:
# Do this in-place:
# Impl. in a way that's easily translatable to C++

def bin_section(line: bytes, start_idx: int, n_bin: int) -> Tuple[int, int]:
    # Bins a section of line 2*n_bin wide into two bytes, starting at start_idx (inclusive):
    # Impl. in a way that's easily translatable to C++
    byte_1: int = 0  # U16
    byte_2: int = 0  # U16
    # Collect all the bytes to be binned:
    for i in range(n_bin):
        byte_1 += line[start_idx+i*2]
        byte_2 += line[start_idx+i*2+1]
    # Perform average:
    byte_1 = (byte_1 // n_bin)
    byte_2 = (byte_2 // n_bin)
    return (byte_1 & 0xFF, byte_2 & 0xFF)


def unbin_section(binned_bytes: bytes, n_bin: int) -> bytes:
    # Expands the given two bytes by `n_bin`
    # Impl. in a way that's easily translatable to C++
    return binned_bytes * n_bin


def line_header_starts_at_idx(line: bytes, idx: int) -> bool:
    # Checks if a line header starts at the given index:
    if (idx + LEN_OF_LINE_HEADER > len(line)):
        return False  # can't, not enough space
    # Line header follows pattern: b'\x11\x22\x33.{1}\x55\x66\x77.{1}\x99\xAA\xBB.{1}\xDD\xEE\xFF'
    return (
        line[idx + 0x00] == 0x11 and
        line[idx + 0x01] == 0x22 and
        line[idx + 0x02] == 0x33 and
        # line[idx + 0x03] == 0x44 and  # data goes here
        line[idx + 0x04] == 0x55 and
        line[idx + 0x05] == 0x66 and
        line[idx + 0x06] == 0x77 and
        # line[idx + 0x07] == 0x88 and  # data goes here
        line[idx + 0x08] == 0x99 and
        line[idx + 0x09] == 0xAA and
        line[idx + 0x0A] == 0xBB and
        # line[idx + 0x0B] == 0xCC and  # data goes here
        line[idx + 0x0C] == 0xDD and
        line[idx + 0x0D] == 0xEE and
        line[idx + 0x0E] == 0xFF
    )


def frame_header_starts_at_idx(line: bytes, idx: int) -> bool:
    # Checks if a line header starts at the given index:
    if (idx + LEN_OF_FRAME_HEADER_DATA > len(line)):
        return False  # can't, not enough space
    # Frame header follow pattern:  b"IRIS-FPGA-FW--RC.{1}\..{1}\..{1}--SENSOR-FRAME-NUM:.{1}--INTERLEAVED-FRAME-NUM:.{1}--IMAGING-MODE:.{1}--"
    # Example: "IRIS-FPGA-FW--RCX.X.X--SENSOR-FRAME-NUM:X--INTERLEAVED-FRAME-NUM:X--IMAGING-MODE:X--"
    # We don't have to check the whole thing, just enough bytes to be sure
    # we're not looking at a chance occurrence in noise:
    return line[idx:idx+14] == b"IRIS-FPGA-FW--"


def find_header_start_in_section(line: bytes, start_idx: int, n_bytes: int) -> int:
    # Check if any of the bytes in the given section are the start of a line or frame header.
    # That is, this checks if the given section of `n_bytes` starting at `start_idx`
    # contains all or part of a line or frame header.
    # If a header is contained in the given section, the index of the first
    # byte of the header is returned.
    # If no header start is found, -1 is returned.
    contains_header: bool = False

    i: int = 0
    while (i < n_bytes and not contains_header):
        if (line_header_starts_at_idx(line, start_idx+i)):
            contains_header = True
        elif (frame_header_starts_at_idx(line, start_idx+i)):
            contains_header = True
        else:
            i += 1

    return start_idx+i if contains_header else -1


def bin_line(n_bin: int, line: bytes | bytearray) -> Tuple[bytes, int]:
    # Performs a bayer-preserving binning operation on the given line.
    # Preserves any line or frame headers encountered.
    # Does this in-place (requires no extra memory).
    # Returns pointer to line and number of bytes in the line which are actually used.

    line = bytearray(line)  # make it mutable

    n_binX2 = n_bin*2  # precompute

    # Index where we're currently reading:
    read_idx: int = 0
    # Number of sections we've binned:
    N_binned: int = 0  # U16
    # Where we're currently writing N_binned (it moves if we have to jump around a header):
    # NOTE: This is U16 so this is the idx of the 1st byte
    N_binned_idx: int = 1
    # Index where we're currently writing (need to leave 3 bytes free at the beginning for binning data):
    write_idx: int = N_binned_idx + 2

    while read_idx < len(line):
        header_start_idx = find_header_start_in_section(
            line, read_idx, n_binX2)

        if (header_start_idx < 0):
            # No header in this section. Perform normal binning:
            binned = bin_section(line, read_idx, n_bin)
            line[write_idx] = binned[0]
            line[write_idx+1] = binned[1]
            N_binned += 1
            # Bump the indices:
            read_idx += n_binX2
            write_idx += 2
        else:
            # Copy all of the remaining bytes before the header and all the bytes in the header:
            # Copy the header:
            header_len: int
            if line[header_start_idx] == 0x11:
                header_len = LEN_OF_LINE_HEADER
            else:
                header_len = LEN_OF_FRAME_HEADER_DATA
            n_bytes_to_copy: int = header_start_idx - read_idx + header_len
            line[write_idx:write_idx + n_bytes_to_copy] \
                = line[read_idx:read_idx+n_bytes_to_copy]
            # Advance read index:
            read_idx += n_bytes_to_copy
            # Apply Nbinned to its previous location, advance its index to after the header, and reset it:
            line[N_binned_idx] = N_binned & 0xFF
            line[N_binned_idx+1] = (N_binned >> 8) & 0xFF
            N_binned = 0
            N_binned_idx = write_idx + n_bytes_to_copy
            # Jump to writing after N_binned:
            write_idx = N_binned_idx + 2

        # If the number of bytes left after all that isn't enough for binning,
        # just copy them directly:
        bytes_left: int = len(line) - read_idx  # I16 (so it can be neg)
        if (bytes_left < n_binX2):
            line[write_idx:write_idx+bytes_left] = line[read_idx:read_idx+bytes_left]
            write_idx += bytes_left  # ack that we wrote these bytes
            read_idx = len(line)  # we've read everything

    # At end, apply metadata about binning process:
    line[0] = n_bin
    line[N_binned_idx] = N_binned & 0xFF
    line[N_binned_idx+1] = (N_binned >> 8) & 0xFF

    return (bytes(line), write_idx)


def unbin_line(line: bytes | bytearray) -> bytes:
    # Reverses bayer-preserving binning on a line, restoring it to its original
    # length and approximately the same contents, with all line and frame
    # headers preserved.
    # Only ever run on GSW so doesn't need to be written to be easily translatable.
    # Extract metadata:
    n_bin = line[0]  # grab the n_bin setting
    # Where the data about the current set of binned sections is:
    N_binned_idx = 1
    read_idx = N_binned_idx + 2  # start reading after header info

    full_line = b''

    while read_idx < len(line):
        # Expand N_binned sections:
        N_binned = line[N_binned_idx] | (line[N_binned_idx+1] << 8)
        for _ in range(N_binned):
            # Unbin every 2 bytes:
            full_line += unbin_section(line[read_idx:read_idx+2], n_bin)
            read_idx += 2
        # See if there's a header in the rest of this:
        header_start_idx = find_header_start_in_section(
            line, read_idx, len(line)-read_idx)
        if header_start_idx < 0:
            # No header in what's left and binning is over, so just copy the rest:
            full_line += line[read_idx:]
            read_idx = len(line)  # we've read everything
        else:
            # There's a header upcoming which is why binning stopped...
            # Copy over everything through the header (incl. anything left before it):
            header_len = LEN_OF_LINE_HEADER if (
                line[header_start_idx] == 0x11) else LEN_OF_FRAME_HEADER_DATA
            full_line += line[read_idx:header_start_idx+header_len]
            # Set the N_binned_idx to right after the header:
            N_binned_idx = header_start_idx + header_len
            # and the read index to right after that:
            read_idx = N_binned_idx + 2

    return full_line


def line_is_homogeneous(line: bytes) -> bool:
    # Returns whether the line contains all of the same value:
    is_homogeneous: bool = True
    i: int = 1
    while (is_homogeneous and i < len(line)):
        is_homogeneous = is_homogeneous and (line[i] == line[0])
        i += 1
    return is_homogeneous
