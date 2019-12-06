/**
 * Interface to the HLS compression implementation
 */

#ifdef __SDK__
#include <xaxicdma.h>
#include <xjpegcompresstiles.h>
#include <xil_cache.h>

#include "sdk.h"
#endif

#include <vector>

#include "dct.h"
#include "util.h"
#include "config.h"
#include "encoder_v2.h"

static float localInput[NR][NC];
static int localOutput[NR][NC];

void jpegCompress(int *output, const uint8_t *input,
        const int W, const int H)
{
	int lastY = -1, lastX = -1;
	int *encodingPtr = output;

    for (int y = 0; y < H; y += NR) {
        for (int x = 0; x < W; x += NC) {
            // fill local buffer with shifted values, pad with 0s
            {
                int iy, fy, ix, fx;
                for (iy = 0, fy = y; fy < MIN(y+NR,H); ++iy, ++fy) {
                    for (ix = 0, fx = x; fx < MIN(x+NC,W); ++ix, ++fx) {
                        localInput[iy][ix] = *(input + fy*W + fx) - 128;
                    }
                    for ( ; ix < NC; ++ix) {
                        localInput[iy][ix] = 0;
                    }
                }
                for ( ; iy < NR; ++iy) {
                    for (ix = 0; ix < NC; ++ix) {
                        localInput[iy][ix] = 0;
                    }
                }
            }

#ifdef __SDK__
            // wait until compress hardware is ready for new inputs
            while (!XJpegcompresstiles_IsReady(&compress_dev));

            // copy data over to input bram
            Xil_DCacheFlushRange((INTPTR)localInput, INPUT_SIZE);
            XAxiCdma_SimpleTransfer(&cdma_dev_input, (UINTPTR)localInput, (UINTPTR)BRAM_INPUT_BASEADDR, INPUT_SIZE, NULL, NULL);

            if (lastX == -1) {
            	// first iteration, wait until transfer is done and launch
            	while (XAxiCdma_IsBusy(&cdma_dev_input));
            	XJpegcompresstiles_Start(&compress_dev);

            	lastX = x;
            	lastY = y;

				continue;
            }

            while (!XJpegcompresstiles_IsIdle(&compress_dev));

            // copy data back
            Xil_DCacheFlushRange((INTPTR)localOutput, OUTPUT_SIZE);
            XAxiCdma_SimpleTransfer(&cdma_dev_output, (UINTPTR)BRAM_OUTPUT_BASEADDR, (UINTPTR)localOutput, OUTPUT_SIZE, NULL, NULL);

            while (XAxiCdma_IsBusy(&cdma_dev_output) || XAxiCdma_IsBusy(&cdma_dev_input));

            XJpegcompresstiles_Start(&compress_dev);
#else
            jpegCompressTiles(localOutput, localInput);
#endif

#ifndef HW_QUANTIZER
            // quantize each tile
            float tile[N][N];
            for (int ti = 0; ti < Tr; ++ti) {
            	for (int tj = 0; tj < Tc; ++tj) {

            		for (int i = 0; i < N; ++i) {
						for (int j = 0; j < N; ++i) {
							tile[i][j] = localOutput[ti*N + i][tj*N + j];
						}
					}

            		quantize_8x8(tile);

            		for (int i = 0; i < N; ++i) {
            			for (int j = 0; j < N; ++j) {
            				localOutput[ti*N + i][tj*N + j] = tile[i][j];
            			}
            		}

            	}
            }
#endif

            // bring results back
#ifdef __SDK__

#ifdef ENABLE_ENCODING
            for (int tr = 0; tr < Tr; ++tr) {
            	for (int tc = 0; tc < Tc; ++tc) {
            		int encodingTemp[N][N];
            		for (int ir = 0; ir < N; ++ir) {
            			for (int ic = 0; ic < N; ++ic) {
            				encodingTemp[ir][ic] = localOutput[tr*N + ir][tc*N + ic];
            			}
            		}
            		encodingPtr += rle(encodingTemp, encodingPtr);
            	}
            }
#else
            for (int iy = 0, fy = lastY; fy < MIN(lastY+NR,H); ++iy, ++fy) {
                for (int ix = 0, fx = lastX; fx < MIN(lastX+NC,W); ++ix, ++fx) {
                    *(output + fy*W + fx) = localOutput[iy][ix];
                }
            }
#endif /* ENABLE_ENCODING */

            lastY = y;
            lastX = x;
#else
            for (int iy = 0, fy = y; fy < MIN(y+NR,H); ++iy, ++fy) {
				for (int ix = 0, fx = x; fx < MIN(x+NC,W); ++ix, ++fx) {
					*(output + fy*W + fx) = localOutput[iy][ix];
				}
			}
#endif /* __SDK__ */

        }
    }

#ifdef __SDK__
    while (!XJpegcompresstiles_IsIdle(&compress_dev));

	// copy data back
    Xil_DCacheFlushRange((INTPTR)localOutput, OUTPUT_SIZE);
	XAxiCdma_SimpleTransfer(&cdma_dev_output, (UINTPTR)BRAM_OUTPUT_BASEADDR, (UINTPTR)localOutput, OUTPUT_SIZE, NULL, NULL);

	while (XAxiCdma_IsBusy(&cdma_dev_output));

	// bring results back
#ifdef ENABLE_ENCODING
	for (int tr = 0; tr < Tr; ++tr) {
		for (int tc = 0; tc < Tc; ++tc) {
			int encodingTemp[N][N];
			for (int ir = 0; ir < N; ++ir) {
				for (int ic = 0; ic < N; ++ic) {
					encodingTemp[ir][ic] = localOutput[tr*N + ir][tc*N + ic];
				}
			}
			encodingPtr += rle(encodingTemp, encodingPtr);
		}
	}
#else
    for (int iy = 0, fy = lastY; fy < MIN(lastY+NR,H); ++iy, ++fy) {
        for (int ix = 0, fx = lastX; fx < MIN(lastX+NC,W); ++ix, ++fx) {
            *(output + fy*W + fx) = localOutput[iy][ix];
        }
    }
#endif /* ENABLE_ENCODING */

#endif /* __SDK__ */

}
