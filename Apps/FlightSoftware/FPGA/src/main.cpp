/**
 * Testbench for the JPEG DCT implementation
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <vector>

#include "dct.h"
#include "util.h"
#include "config.h"
#include "encoder_v2.h"
#include "decoder.h"
#include "images.h"

#ifdef __SDK__
#include "sdk.h"
#include <xtime_l.h>
#endif

#define FULL_WIDTH 2592
#define FULL_HEIGHT 1944

#define FULL_IMAGE_WIDTH 512

template <typename T>
void printMat(const T *mat, const int W, const int H) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
        	printf("%f\t", (float)mat[y*W + x]);
        	fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
}

static float localInput[N][N];
static float localOutput[N][N];
static int quantizeOutput[N][N];
void jpegCompressReference(int *output, const uint8_t *input,
        const int W, const int H)
{
	int *encoderPtr = output;

    for (int y = 0; y < H; y += N) {
        for (int x = 0; x < W; x += N) {
            // fill local buffer with shifted values
            {
                int iy, fy, ix, fx;
                for (iy = 0, fy = y; fy < MIN(y+N,H); ++iy, ++fy) {
                    for (ix = 0, fx = x; fx < MIN(x+N,W); ++ix, ++fx) {
                        localInput[iy][ix] = *(input + fy*W + fx) - 128;
                    }
                    for ( ; ix < N; ++ix) {
                        localInput[iy][ix] = 0;
                    }
                }
                for ( ; iy < N; ++iy) {
                    for (ix = 0; ix < N; ++ix) {
                        localInput[iy][ix] = 0;
                    }
                }
            }

            // fill output with 0s
            {
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < N; ++j) {
                        localOutput[i][j] = 0;
                    }
                }
            }

            // do the compute on this block
            DCT_8x8(localOutput, localInput);
            quantize_8x8(localOutput);

            // bring results back
#ifdef ENABLE_ENCODING
            for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					quantizeOutput[i][j] = localOutput[i][j];
				}
			}
            encoderPtr += rle(quantizeOutput, encoderPtr);
#else
            for (int iy = 0, fy = y; fy < MIN(y+N,H); ++iy, ++fy) {
                for (int ix = 0, fx = x; fx < MIN(x+N,W); ++ix, ++fx) {
                    *(output + fy*W + fx) = localOutput[iy][ix];
                }
            }
#endif /* ENABLE_ENCODING */
        }
    }
}

#ifdef SMALL_TEST
int refSmallOutput[8*8];
int testSmallOutput[8*8];
uint8_t smallInput[8*8] =
	{
		154, 123, 123, 123, 123, 123, 123, 136,
		192, 180, 136, 154, 154, 154, 136, 110,
		254, 198, 154, 154, 180, 154, 123, 123,
		239, 180, 136, 180, 180, 166, 123, 123,
		180, 154, 136, 167, 166, 149, 136, 136,
		128, 136, 123, 136, 154, 180, 198, 154,
		123, 105, 110, 149, 136, 136, 180, 166,
		110, 136, 123, 123, 123, 136, 154, 136
	};
#endif

#ifdef MEDIUM_TEST

#define MEDIUM_WIDTH 16
#define MEDIUM_HEIGHT 16

uint8_t mediumInput[MEDIUM_WIDTH*MEDIUM_HEIGHT];
int refMediumOutput[2*MEDIUM_WIDTH*MEDIUM_HEIGHT] = { 0 };
int testMediumOutput[2*MEDIUM_WIDTH*MEDIUM_HEIGHT] = { 0 };

#endif

#ifdef LARGE_TEST
uint8_t largeInput[FULL_WIDTH*FULL_HEIGHT];
int refLargeOutput[2*FULL_WIDTH*FULL_HEIGHT] = { 0 };
int testLargeOutput[2*FULL_WIDTH*FULL_HEIGHT] = { 0 };
#endif

#ifdef LIVING_ROOM
int livingRoomOutput[LIVINGROOM_HEIGHT*LIVINGROOM_WIDTH] = { 0 };
float livingRoomDecoded[LIVINGROOM_WIDTH*LIVINGROOM_HEIGHT] = { 0 };
#endif

int main() {

#ifdef __SDK__
	printf("*************************\n");
	printf("**Running ZYNQ ARM Code**\n");
	printf("*************************\n\n");
#else
	printf("*********************\n");
	printf("**Running Host Code**\n");
	printf("*********************\n");
#endif

#ifdef __SDK__
	int status = 0;
	init_platform(status);
	if (status) {
		printf("platform init failed, bailing out\n");
		return XST_FAILURE;
	}
	printf("platform init complete\n\n");
#endif

	printf("Tr=%d, Tc=%d, Q=%f, CUT=%d\n\n", Tr, Tc, Q, CUTOFF_IDX);

#ifdef SMALL_TEST
    jpegCompressReference(refSmallOutput, smallInput, 8, 8);
    jpegCompress(testSmallOutput, smallInput, 8, 8);
#endif

#ifdef MEDIUM_TEST
    for (int i = 0; i < MEDIUM_HEIGHT; ++i) {
    	for (int j = 0; j < MEDIUM_WIDTH; ++j) {
    		mediumInput[i*MEDIUM_WIDTH + j] =
    				((float)rand() / (float)RAND_MAX) * UINT8_MAX;
    	}
    }

    jpegCompressReference(refMediumOutput, mediumInput, MEDIUM_WIDTH, MEDIUM_HEIGHT);
    jpegCompress(testMediumOutput, mediumInput, MEDIUM_WIDTH, MEDIUM_HEIGHT);
#endif

#ifdef LARGE_TEST
    // first generate data for large test
    for (int i = 0; i < FULL_HEIGHT; ++i) {
        for (int j = 0; j < FULL_WIDTH; ++j) {
            largeInput[i*FULL_WIDTH + j] =
                ((float)rand() / (float)RAND_MAX) * UINT8_MAX;
        }
    }

#ifndef NO_TIMING
    printf("Timing reference execution\n");
#ifdef __SDK__
    XTime cpuStart, cpuStop, kernStart, kernStop;
    XTime_GetTime(&cpuStart);
#else
    struct timeval cpuStart, cpuStop;
    gettimeofday(&cpuStart, NULL);
#endif
#endif /* NO_TIMING */

    jpegCompressReference(refLargeOutput, largeInput, FULL_WIDTH, FULL_HEIGHT);

#ifndef NO_TIMING
#ifdef __SDK__
    XTime_GetTime(&cpuStop);
    double cpuTime = (double)(cpuStop - cpuStart) / COUNTS_PER_SECOND;
#else
    gettimeofday(&cpuStop, NULL);
    printf("Runtime = %0.1f (us)\n\n", (cpuStop.tv_sec - cpuStart.tv_sec)*1e6 + (cpuStop.tv_usec - cpuStart.tv_usec));
#endif
#endif /* NO_TIMING */

#ifndef NO_TIMING
    printf("Timing kernel\n");
#ifdef __SDK__
    XTime_GetTime(&kernStart);
#else
    struct timeval start, stop;
    gettimeofday(&start, NULL);
#endif
#endif /* NO_TIMING */

    jpegCompress(testLargeOutput, largeInput, FULL_WIDTH, FULL_HEIGHT);

#ifndef NO_TIMING
#ifdef __SDK__
    XTime_GetTime(&kernStop);
    double kernTime = (double)(kernStop - kernStart) / COUNTS_PER_SECOND;
#else
    gettimeofday(&stop, NULL);
#endif
#endif /* NO_TIMING */

#endif /* LARGE_TEST */

#ifdef LIVING_ROOM
    jpegCompress(livingRoomOutput, livingRoom, LIVINGROOM_WIDTH, LIVINGROOM_HEIGHT);

#ifdef ENABLE_ENCODING
    // now decode the living room
    float tile[N][N];
    int *decodePtr = livingRoomOutput;
    for (int ti = 0; ti < LIVINGROOM_HEIGHT; ti += N) {
    	for (int tj = 0; tj < LIVINGROOM_WIDTH; tj += N) {
    		decodePtr += decoderv1(decodePtr, tile);

    		for (int i = 0; i < N; ++i) {
    			for (int j = 0; j < N; ++j) {
    				livingRoomDecoded[(ti + i)*LIVINGROOM_WIDTH + (tj + j)] = tile[i][j];
    			}
    		}
    	}
    }
#endif

    // go ahead and print these now
    printf("living room encoded:\n");
    printMat(livingRoomOutput, LIVINGROOM_WIDTH, LIVINGROOM_HEIGHT);
    printf("\n");

#ifdef ENABLE_ENCODING
    printf("living room decoded:\n");
    printMat(livingRoomDecoded, LIVINGROOM_WIDTH, LIVINGROOM_HEIGHT);
    printf("\n");
#endif

#endif /* LIVING_ROOM */

#ifdef PRINTING
#ifdef SMALL_TEST
    printf("small reference:\n");
    printMat(refSmallOutput, 8, 8);
    printf("\n");

    printf("small test:\n");
    printMat(testSmallOutput, 8, 8);
    printf("\n");
#endif /* SMALL_TEST */

#ifdef MEDIUM_TEST
    printf("medium reference:\n");
    printMat(refMediumOutput, MEDIUM_WIDTH, MEDIUM_HEIGHT);
    printf("\n");

    printf("medium test:\n");
    printMat(testMediumOutput, MEDIUM_WIDTH, MEDIUM_HEIGHT);
    printf("\n");
#endif /* MEDIUM_TEST */

#ifdef LARGE_TEST
    printf("large reference:\n");
    printMat(refLargeOutput, FULL_IMAGE_WIDTH, FULL_IMAGE_WIDTH);
    printf("\n");

    printf("large test:\n");
    printMat(testLargeOutput, FULL_IMAGE_WIDTH, FULL_IMAGE_WIDTH);
    printf("\n");
#endif /* LARGE_TEST */
#endif /* PRINTING */

    // print timing data just before correctness (if enabled)
#if defined(LARGE_TEST) && !defined(NO_TIMING)
#ifdef __SDK__
    printf("CPU execution time: %lf (sec)\n", cpuTime);
    printf("Kernel execution time: %lf (sec)\n\n", kernTime);
#else
    printf("Runtime = %0.1f (us)\n\n", (stop.tv_sec - start.tv_sec)*1e6 + (stop.tv_usec - start.tv_usec));
#endif
#endif

    // correctness should be the last thing
#ifdef CHECK_OUTPUT
    const char *pass = "pass", *fail = "fail";
#ifdef SMALL_TEST
    printf("small test: %s\n",
    		(checkClose(testSmallOutput, refSmallOutput, 8*8)) ? pass : fail);
#endif

#ifdef MEDIUM_TEST
    printf("medium test: %s\n",
    		(checkClose(testMediumOutput, refMediumOutput, MEDIUM_WIDTH*MEDIUM_HEIGHT)) ? pass : fail);
#endif

#ifdef LARGE_TEST
    printf("large test: %s\n",
    		(checkClose(testLargeOutput, refLargeOutput, FULL_WIDTH*FULL_HEIGHT)) ? pass : fail);
#endif
#endif /* CHECK_OUTPUT */

    return 0;
}
