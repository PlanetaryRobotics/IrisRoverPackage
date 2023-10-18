/*
Toolkit for compressing & decompressing images both losslessly,
using heatshrink, and lossy using a bayer-preserving binning procedure,
similar to how the Aptina sensor does it, but in a way that preserves
RC11-style metadata embedded in the images by the RC11+ FPGA FW.

Aptina-style bayer-preserving binning is used as an easy method of acceptably
lossy compression.

Counterpart to `image_compression_toolkit.py` in the GSW GDS.

@author: Connor W. Colombo (CMU)
@last-updated: 10/16/2023
*/

#ifndef IRIS_IMAGE__COMPRESSION
#define IRIS_IMAGE__COMPRESSION

#include <stdint.h>
#include <Fw/Cfg/Config.hpp>

// Comment this out to remove the ability to use compression (heatshrink) and save a couple kB of RAM.
// Functions requesting compression will still work if this is removed, just compression won't happen.
#define OFFER_COMPRESSION

namespace IrisImage
{
    // Data for FPGA FW Headers:
    const uint8_t LEN_OF_LINE_HEADER = 15;
    const uint8_t LEN_OF_FRAME_HEADER_DATA = 84;
    // Compare just the start of a frame header for time:
    const uint8_t START_OF_FRAME_HEADER[] = "IRIS-FPGA-FW--";
    const uint8_t START_OF_FRAME_HEADER_LEN = 14;

    bool line_is_homogeneous(uint8_t *line, uint16_t lineLen);
    bool line_header_starts_at_idx(uint8_t *line, uint16_t lineLen, uint16_t idx);
    bool frame_header_starts_at_idx(uint8_t *line, uint16_t lineLen, uint16_t idx);
    int16_t find_header_start_in_section(uint8_t *line, uint16_t lineLen, uint16_t start_idx, uint16_t n_bytes);

    void bin_section(uint8_t n_bin, uint8_t *line, uint16_t lineLen, uint16_t read_idx, uint16_t write_idx);
    uint16_t bin_line(uint8_t n_bin, uint8_t *line, uint16_t lineLen, const uint16_t data_start_idx = 0);
    uint16_t heatshrink_buffer(uint8_t *inputDataToCompress, uint16_t inputSize, uint8_t *outputBuffer, uint16_t outputBufferSize);
    uint16_t compressAndBinLine(uint8_t *line, uint16_t lineLen, uint8_t n_bin, bool use_heatshrink, bool *binningOccurred, bool *compressionOccurred);

}

#endif // IRIS_IMAGE__COMPRESSION
