#include "IrisImageCompression.hpp"

#include <stddef.h>
#include <string.h>

#include "heatshrink/heatshrink_encoder.h"

#if HEATSHRINK_DYNAMIC_ALLOC
#error HEATSHRINK_DYNAMIC_ALLOC should not be used. Please set it to 0.
#endif

namespace IrisImage
{
#ifdef OFFER_COMPRESSION
    static heatshrink_encoder hse;

    // Static buffer to store compression data:
    // Needs to be at least the same size as an image line
    // (or, at it needs to be as big as the maximum size we'll accept for a
    // compressed line. If we only ever want to compress things up to X bytes,
    // then X is an okay setting).
    static const uint16_t COMPRESSION_BUFFER_LEN = 512 * 3;
    static uint8_t g_compression_buffer[COMPRESSION_BUFFER_LEN];

    /*
    That compresses that data given in `inputDataToCompress` with size
    `inputSize` and puts the compressed data into `outputBuffer`, making sure
    not to exceed `outputBufferSize`.

    Based on `compress_and_expand_and_check` in `test_heatshrink_static.c`.

    Returns the size of the compressed buffer.
    If the returned value is 0xFFFF, compression failed.
    */
    uint16_t heatshrink_buffer(
        uint8_t *inputDataToCompress, uint16_t inputSize,
        uint8_t *outputBuffer, uint16_t outputBufferSize)
    {
        // Statically allocate working variables so they don't end up on the stack:
        // (we don't care about re-entrancy in this case)
        static HSE_poll_res poll_res;
        static uint16_t sunk, polled;
        static size_t count;
        static const uint16_t FAIL_VALUE = 0xFFFF;

        // Reset Heatshrink Encoder SM:
        heatshrink_encoder_reset(&hse);

        // Pointers to keep track of data input and output
        sunk = 0;   // amount of raw data pushed into encoder
        polled = 0; // amount of compressed data pulled out of encoder

        // Temporary variables to hold function call results:
        count = 0;

        while (sunk < inputSize)
        {
            // Feed as much data as we can from inputDataToCompress into the encoder
            // (Sink data into the Encoder):
            if (heatshrink_encoder_sink(&hse, &inputDataToCompress[sunk], inputSize - sunk, &count) < 0)
            {
                // Sink failed. Can't compress. Stop & return failure:
                return FAIL_VALUE;
            }
            sunk += count;

            if (sunk == inputSize)
            {
                if (heatshrink_encoder_finish(&hse) != HSER_FINISH_MORE)
                {
                    // HSE in bad state. Return failure.
                    return FAIL_VALUE;
                }
            }

            // Turn the crank on the encoder FSM,
            // pulling compressed data out as it becomes ready:
            do
            {
                poll_res = heatshrink_encoder_poll(&hse, &outputBuffer[polled], outputBufferSize - polled, &count);
                if (poll_res < 0)
                {
                    // HSE in bad state. Stop & return failure:
                    return FAIL_VALUE;
                }

                polled += count;
            } while (poll_res == HSER_POLL_MORE);

            if (polled >= outputBufferSize)
            {
                // Compression used up all of the allowed space.
                // Have to consider this a failure.
                return FAIL_VALUE;
            }

            if (poll_res != HSER_POLL_EMPTY)
            {
                // HSE in bad state. Stop & return failure:
                return FAIL_VALUE;
            }

            // When all input data is sunk, finish the encoding:
            if (sunk == inputSize)
            {
                if (heatshrink_encoder_finish(&hse) != HSER_FINISH_DONE)
                {
                    // Couldn't wrap up compression. Return fail:
                    return FAIL_VALUE;
                }
            }
        }

        // Return size of compressed data:
        return polled;
    }
#endif // OFFER_COMPRESSION

    // Returns whether the line contains all of the same value:
    bool line_is_homogeneous(uint8_t *line, uint16_t lineLen)
    {
        bool is_homogeneous = true;
        uint16_t i = 1;
        while (is_homogeneous && i < lineLen)
        {
            is_homogeneous = is_homogeneous && (line[i] == line[0]);
            i++;
        }

        return is_homogeneous;
    }

    bool line_header_starts_at_idx(uint8_t *line, uint16_t lineLen, uint16_t idx)
    {
        // Checks if a line header starts at the given index
        const uint16_t LEN_OF_LINE_HEADER = 15;
        if (idx + LEN_OF_LINE_HEADER > lineLen)
        {
            return false; // can't, not enough space
        }
        // Line header follows pattern: b'\x11\x22\x33.{1}\x55\x66\x77.{1}\x99\xAA\xBB.{1}\xDD\xEE\xFF'
        return (
            line[idx + 0x00] == 0x11 &&
            line[idx + 0x01] == 0x22 &&
            line[idx + 0x02] == 0x33 &&
            // line[idx + 0x03] == 0x44,  // data goes here
            line[idx + 0x04] == 0x55 &&
            line[idx + 0x05] == 0x66 &&
            line[idx + 0x06] == 0x77 &&
            // line[idx + 0x07] == 0x88,  // data goes here
            line[idx + 0x08] == 0x99 &&
            line[idx + 0x09] == 0xAA &&
            line[idx + 0x0A] == 0xBB &&
            // line[idx + 0x0B] == 0xCC,  // data goes here
            line[idx + 0x0C] == 0xDD &&
            line[idx + 0x0D] == 0xEE &&
            line[idx + 0x0E] == 0xFF);
    }

    bool frame_header_starts_at_idx(uint8_t *line, uint16_t lineLen, uint16_t idx)
    {
        // Checks if a frame header starts at the given index
        if (idx + LEN_OF_FRAME_HEADER_DATA > lineLen)
        {
            return false; // can't, not enough space
        }

        // Only checking enough bytes to be sure we're not looking at a chance occurrence in noise
        return memcmp(line + idx, START_OF_FRAME_HEADER, START_OF_FRAME_HEADER_LEN) == 0;
    }

    /*
    Check if any of the bytes in the given section are the start of a line or frame header.
    That is, this checks if the given section of `n_bytes` starting at `start_idx`
    contains all or part of a line or frame header.

    If a header is contained in the given section, the index of the first
    byte of the header is returned.
    If no header start is found, -1 is returned.
    */
    int16_t find_header_start_in_section(uint8_t *line, uint16_t lineLen, uint16_t start_idx, uint16_t n_bytes)
    {
        bool contains_header = false;
        uint16_t i = 0;

        while (i < n_bytes && !contains_header)
        {
            if (line_header_starts_at_idx(line, lineLen, start_idx + i))
            {
                contains_header = true;
            }
            else if (frame_header_starts_at_idx(line, lineLen, start_idx + i))
            {
                contains_header = true;
            }
            else
            {
                i++;
            }
        }

        return contains_header ? (int16_t)(start_idx + i) : (int16_t)-1;
    }

    /*
    Bins a section of line 2*n_bin wide into two bytes, starting at read_idx (inclusive).
    Writes results in-place in `line` at `write_idx`.
    */
    void bin_section(uint8_t n_bin, uint8_t *line, uint16_t lineLen, uint16_t read_idx, uint16_t write_idx)
    {
        // U16s for holding sums to create the averages:
        static uint16_t byte_1, byte_2;
        static uint8_t i;
        byte_1 = 0;
        byte_2 = 0;

        // Collect all the bytes to be binned:
        for (i = 0; i < n_bin; ++i)
        {
            byte_1 += line[read_idx + i * 2];
            byte_2 += line[read_idx + i * 2 + 1];
        }

        // Perform average:
        byte_1 = (byte_1 / n_bin);
        byte_2 = (byte_2 / n_bin);

        // Write the resulting bytes back to the line:
        line[write_idx] = byte_1 & 0xFF;
        line[write_idx + 1] = byte_2 & 0xFF;
    }

    /*
    Perform a bayer-preserving binning operation on the given line.
    Preserves any line or frame headers encountered.
    Operates in-place (requires no extra memory).

    Writing is started at data_start_idx. This can be used if you want to
    reserve a couple of bytes at the start for other metadata without needing
    to copy or shift this data.

    NOTE: For this in-place transaction to work and to avoid binning header
    data, the following must be true:
        starting position + 3B of header + 2B binned data < bin read size
    that is:
        `data_start_idx + 3 + 2 < n_bin*2`
    If this equality is not obeyed, binning won't be attempted.

    Returns the number of bytes in the line which are actually used.
    */
    uint16_t bin_line(uint8_t n_bin, uint8_t *line, uint16_t lineLen, const uint16_t data_start_idx)
    {
        // Statically allocate working variables so they don't end up on the stack:
        // (we don't care about re-entrancy in this case)
        static uint16_t n_binX2, read_idx, N_binned, N_binned_idx, write_idx;
        static uint16_t header_len, n_bytes_to_copy;
        static int16_t header_start_idx, bytes_left;
        static uint16_t i;

        // Sanity check inputs:
        if (n_bin < 2)
        {
            // This will do nothing, so just leave it as is:
            return lineLen;
        }

        // Precompute n_bin * 2
        n_binX2 = n_bin * 2;

        // Initialize indices
        read_idx = 0;
        N_binned = 0;
        N_binned_idx = data_start_idx + 1;
        write_idx = N_binned_idx + 2;

        if ((write_idx + 2) >= n_binX2)
        {
            // `data_start_idx` is too big, we'll read header data while
            // binning.
            // Need to drop `data_start_idx` or bump `n_bin` to fix this.
            // Don't attempt binning:
            return lineLen;
        }

        if (n_bin * 2)

            while (read_idx < lineLen)
            {
                header_start_idx = find_header_start_in_section(line, lineLen, read_idx, n_binX2);

                if (header_start_idx < 0)
                {
                    // No header in this section. Perform normal binning:
                    bin_section(n_bin, line, lineLen, read_idx, write_idx);
                    N_binned += 1;
                    // Bump the indices:
                    read_idx += n_binX2;
                    write_idx += 2;
                }
                else
                {
                    // Copy all of the remaining bytes before the header and all the bytes in the header
                    header_len = (line[header_start_idx] == 0x11) ? LEN_OF_LINE_HEADER : LEN_OF_FRAME_HEADER_DATA;
                    n_bytes_to_copy = header_start_idx - read_idx + header_len;

                    for (i = 0; i < n_bytes_to_copy; ++i)
                    {
                        line[write_idx + i] = line[read_idx + i];
                    }

                    // Advance read index:
                    read_idx += n_bytes_to_copy;

                    // Apply N_binned to its previous location, advance its index to after the header, and reset it:
                    line[N_binned_idx] = N_binned & 0xFF;
                    line[N_binned_idx + 1] = (N_binned >> 8) & 0xFF;
                    N_binned = 0;
                    N_binned_idx = write_idx + n_bytes_to_copy;

                    // Jump to writing after N_binned:
                    write_idx = N_binned_idx + 2;
                }

                // If the number of bytes left after all that isn't enough for binning, just copy them directly:
                bytes_left = lineLen - read_idx; // I16 (so it can be neg)

                if (bytes_left < n_binX2)
                {
                    for (i = 0; i < bytes_left; ++i)
                    {
                        line[write_idx + i] = line[read_idx + i];
                    }

                    write_idx += bytes_left; // ack that we wrote these bytes
                    read_idx = lineLen;      // we've read everything
                }
            }

        // At end, apply metadata about binning process
        line[data_start_idx] = n_bin;
        line[N_binned_idx] = N_binned & 0xFF;
        line[N_binned_idx + 1] = (N_binned >> 8) & 0xFF;

        return write_idx;
    }

    /*
    Bin the given line in place according to the given binning setting.

    Also applies losslessly compresses the line using heatshrink if desired.
    The compressed result is only used if it results in a decrease in size.

    The first byte of the buffer will indicated whether or not compression is
    applied on the line.

    Lines containing homogeneous data are given a simple RLE: "ALL" followed by
    a U16 of the length of the line and a U8 of the homogeneous byte.

    Returns the length of data in line that's now used (should be <=).
    Sets `binningOccurred` based on if binning actually occurred and the data in `line` is now binned.
    Sets `compressionOccurred` based on if compression actually occurred and the data in `line` is now compressed.
    */
    uint16_t compressAndBinLine(
        uint8_t *line, uint16_t lineLen,
        uint8_t n_bin, bool use_heatshrink,
        bool *binningOccurred, bool *compressionOccurred //
    )
    {
        static uint16_t binnedLen, compressedLen;

        // If line is homogeneous, use simple RLE (if it'll reduce the line size):
        if (line_is_homogeneous(line, lineLen) && lineLen > 6)
        {
            // "ALL" in ASCII (an identifiable header) + line length (2 bytes) + char to repeat
            line[0] = 'A';
            line[1] = 'L';
            line[2] = 'L';
            line[3] = lineLen & 0xFF;
            line[4] = (lineLen >> 8) & 0xFF;
            // Keep line[5] the same to serve as the repeat char (don't need to do anything for that).
            return 6;
        }
        else
        {
            // (Attempt to) bin the line:
            binnedLen = bin_line(n_bin, line, lineLen);
            *binningOccurred = (binnedLen < lineLen);

            *compressionOccurred = false;
#ifdef OFFER_COMPRESSION
            if (use_heatshrink)
            {
                // Attempt compression:
                // NOTE: If we need to save space, we could get rid of the
                // dedicated compression buffer, expand line buffer to
                // 1.5*line_size + 4 (from heatshrink), and compress in-place.
                // ^ Does heatshrink support in-place compression? (maybe not...)
                // Can also just drop COMPRESSION_BUFFER_LEN so we only support
                // very tight compression.
                compressedLen = heatshrink_buffer(line, binnedLen, g_compression_buffer, COMPRESSION_BUFFER_LEN);
                // Only use compressed result if it resulted in size reduction:
                if (compressedLen < binnedLen)
                {
                    memcpy(line, g_compression_buffer, compressedLen);
                    *compressionOccurred = true;
                }
            }
#endif // OFFER_COMPRESSION
        }

        if (*compressionOccurred)
        {
            return compressedLen;
        }
        else
        {
            return binnedLen;
        }
    }

}
