/**
 * Handle zynq platform initialization
 */

#include <stdio.h>

#include <xparameters.h>
#include <xaxicdma.h>
#include <xjpegcompresstiles.h>

#include "sdk.h"

XAxiCdma 		cdma_dev_input;
XAxiCdma_Config *cdma_config_input;
XAxiCdma 		cdma_dev_output;
XAxiCdma_Config	*cdma_config_output;

XJpegcompresstiles 			compress_dev;
XJpegcompresstiles_Config	*compress_config;

void init_platform(int &error) {
	int status;

	compress_config = XJpegcompresstiles_LookupConfig(COMPRESS_ID);
	status = XJpegcompresstiles_CfgInitialize(&compress_dev, compress_config);
	if (status != XST_SUCCESS) {
		printf("ERROR: XJpegcompresstiles_dev setup failed\n");
		error++;
	}

	cdma_config_input = XAxiCdma_LookupConfig(COMPRESS_INPUT_ID);
	status = XAxiCdma_CfgInitialize(&cdma_dev_input, cdma_config_input, COMPRESS_INPUT_BASEADDR);
	if (status != XST_SUCCESS) {
		printf("ERROR: CDMA INPUT setup failed\n");
		error++;
	}

	cdma_config_output = XAxiCdma_LookupConfig(COMPRESS_OUTPUT_ID);
	status = XAxiCdma_CfgInitialize(&cdma_dev_output, cdma_config_output, COMPRESS_OUTPUT_BASEADDR);
	if (status != XST_SUCCESS) {
		printf("ERROR: CDMA OUTPUT setup failed\n");
		error++;
	}

	// do a wait on everything here to make sure they're ready
	while (XAxiCdma_IsBusy(&cdma_dev_input));
	while (XAxiCdma_IsBusy(&cdma_dev_output));
	while (!XJpegcompresstiles_IsReady(&compress_dev));

}
