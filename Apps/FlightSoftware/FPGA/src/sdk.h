/**
 * Zynq initialization declarations
 */

#ifndef SDK_H
#define SDK_H

#include <xaxicdma.h>
#include <xjpegcompresstiles.h>

#include "dct.h"

#define COMPRESS_INPUT_ID XPAR_HIER_BRAM_1_AXI_CDMA_0_DEVICE_ID
#define COMPRESS_INPUT_BASEADDR XPAR_HIER_BRAM_1_AXI_CDMA_0_BASEADDR

#define COMPRESS_OUTPUT_ID XPAR_HIER_BRAM_0_AXI_CDMA_0_DEVICE_ID
#define COMPRESS_OUTPUT_BASEADDR XPAR_HIER_BRAM_0_AXI_CDMA_0_BASEADDR

#define COMPRESS_ID XPAR_JPEGCOMPRESSTILES_0_DEVICE_ID

#define HIER_BRAM_0_BASEADDR 0xc0000000
#define HIER_BRAM_1_BASEADDR 0xc2000000
#define BRAM_INPUT_BASEADDR HIER_BRAM_1_BASEADDR
#define BRAM_OUTPUT_BASEADDR HIER_BRAM_0_BASEADDR

#define INPUT_SIZE ((NR)*(NC)*sizeof(float))
#define OUTPUT_SIZE ((NR)*(NC)*sizeof(int))

extern XAxiCdma 		cdma_dev_input;
extern XAxiCdma_Config 	*cdma_config_input;
extern XAxiCdma 		cdma_dev_output;
extern XAxiCdma_Config	*cdma_config_output;

extern XJpegcompresstiles 			compress_dev;
extern XJpegcompresstiles_Config	*compress_config;

void init_platform(int &error);

#endif /* SDK_H */
