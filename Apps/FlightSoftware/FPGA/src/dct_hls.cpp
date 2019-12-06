/**
 * HLS implementation of the DCT
 */

#include <math.h>

#include "dct.h"
#include "util.h"
#include "config.h"

//#define ROUND(x) ((x < 0.f) ? (int)(x - 0.5f) : (int)(x + 0.5f))
#define ROUND(x) ((x < 0.f) ? (x - 0.5f) : (x + 0.5f))

const float DCT_MATRIX[N][N] = {
        { 0.35355339059327373, 0.35355339059327373, 0.35355339059327373, 0.35355339059327373, 0.35355339059327373, 0.35355339059327373, 0.35355339059327373, 0.35355339059327373 },
        { 0.4903926402016152, 0.4157348061512726, 0.27778511650980114, 0.09754516100806417, -0.0975451610080641, -0.277785116509801, -0.4157348061512727, -0.4903926402016152 },
        { 0.46193976625564337, 0.19134171618254492, -0.19134171618254486, -0.46193976625564337, -0.4619397662556434, -0.19134171618254517, 0.191341716182545, 0.46193976625564326 },
        { 0.4157348061512726, -0.0975451610080641, -0.4903926402016152, -0.2777851165098011, 0.2777851165098009, 0.4903926402016152, 0.09754516100806439, -0.41573480615127256 },
        { 0.3535533905932738, -0.35355339059327373, -0.35355339059327384, 0.3535533905932737, 0.35355339059327384, -0.35355339059327334, -0.35355339059327356, 0.3535533905932733 },
        { 0.27778511650980114, -0.4903926402016152, 0.09754516100806415, 0.41573480615127273, -0.41573480615127256, -0.09754516100806401, 0.4903926402016153, -0.27778511650980076 },
        { 0.19134171618254492, -0.4619397662556434, 0.46193976625564326, -0.19134171618254495, -0.19134171618254528, 0.46193976625564337, -0.4619397662556432, 0.19134171618254478 },
        { 0.09754516100806417, -0.2777851165098011, 0.41573480615127273, -0.4903926402016153, 0.4903926402016152, -0.4157348061512725, 0.27778511650980076, -0.09754516100806429 }
	};

/*
const int q50[N][N] = {
		{ 16, 11, 10, 16, 24,  40,  51,  61  },
		{ 12, 12, 14, 19, 26,  58,  60,  55  },
		{ 14, 13, 16, 24, 40,  57,  69,  56  },
		{ 14, 17, 22, 29, 51,  87,  80,  62  },
		{ 18, 22, 37, 56, 68,  109, 103, 77  },
		{ 24, 35, 55, 64, 81,  104, 113, 92  },
		{ 49, 64, 78, 87, 103, 121, 120, 101 },
		{ 72, 92, 95, 98, 112, 100, 103, 99  }
	};
*/

const float q50I[N][N] = {
		{0.0625,0.09090909090909091,0.1,0.0625,0.041666666666666664,0.025,0.0196078431372549,0.01639344262295082},
		{0.08333333333333333,0.08333333333333333,0.07142857142857142,0.05263157894736842,0.038461538461538464,0.017241379310344827,0.016666666666666666,0.01818181818181818},
		{0.07142857142857142,0.07692307692307693,0.0625,0.041666666666666664,0.025,0.017543859649122806,0.014492753623188406,0.017857142857142856},
		{0.07142857142857142,0.058823529411764705,0.045454545454545456,0.034482758620689655,0.0196078431372549,0.011494252873563218,0.0125,0.016129032258064516},
		{0.05555555555555555,0.045454545454545456,0.02702702702702703,0.017857142857142856,0.014705882352941176,0.009174311926605505,0.009708737864077669,0.012987012987012988},
		{0.041666666666666664,0.02857142857142857,0.01818181818181818,0.015625,0.012345679012345678,0.009615384615384616,0.008849557522123894,0.010869565217391304},
		{0.02040816326530612,0.015625,0.01282051282051282,0.011494252873563218,0.009708737864077669,0.008264462809917356,0.008333333333333333,0.009900990099009901},
		{0.013888888888888888,0.010869565217391304,0.010526315789473684,0.01020408163265306,0.008928571428571428,0.01,0.009708737864077669,0.010101010101010102}
};

const int encodeIdx[N][N] = {
		{ 0,  1,  5,  6,  14, 15, 27, 28 },
		{ 2,  4,  7,  13, 16, 26, 29, 42 },
		{ 3,  8,  12, 17, 25, 30, 41, 43 },
		{ 9,  11, 18, 24, 31, 40, 44, 53 },
		{ 10, 19, 23, 32, 39, 45, 52, 54 },
		{ 20, 22, 33, 38, 46, 51, 55, 60 },
		{ 21, 34, 37, 47, 50, 56, 59, 61 },
		{ 35, 36, 48, 49, 57, 58, 62, 63 }
	};

void quantize_8x8(float image_8x8[N][N]) {
#pragma HLS PIPELINE

	const float multiplier = (float)(100.f - Q) / 50.f;
	for (int i = 0; i < N; i++) {
#pragma HLS UNROLL
		for (int j = 0; j < N; j++) {
#pragma HLS UNROLL
			// if the value is passed our cutoff, just set it to 0
			const float qValue = (float)q50I[i][j] * multiplier;
			image_8x8[i][j] = (encodeIdx[i][j] < CUTOFF_IDX) ? roundf(image_8x8[i][j] * qValue) : 0.f;
		}
	}
}

void DCT_8x8(float output[8][8], const float input[8][8]) {
	float intermediate[8][8];
#pragma HLS ARRAY_PARTITION variable=intermediate complete dim=2
#pragma HLS ARRAY_PARTITION variable=intermediate complete dim=1


	for (int i = 0; i < 8; ++i) {
#pragma HLS UNROLL
		for (int j = 0; j < 8; ++j) {
#pragma HLS UNROLL
			intermediate[i][j] = 0;
		}
	}

	// intermediate = T x M
TxM:
	for (int k = 0; k < 8; ++k) {
#pragma HLS PIPELINE
    	for (int i = 0; i < 8; ++i) {
#pragma HLS UNROLL
    		for (int j = 0; j < 8; ++j) {
#pragma HLS UNROLL
				intermediate[i][j] += DCT_MATRIX[i][k] * input[k][j];
			}
		}
	}

    // output = intermediate x T' = (TxM)xT'
IxTt:
	for (int k = 0; k < 8; ++k) {
#pragma HLS PIPELINE
		for (int i = 0; i < 8; ++i) {
#pragma HLS UNROLL
			for (int j = 0; j < 8; ++j) {
#pragma HLS UNROLL
				output[i][j] += intermediate[i][k] * DCT_MATRIX[j][k];
			}
		}
	}
}

void jpegCompressTiles(int output[NR][NC], const float input[NR][NC]) {
#pragma HLS ARRAY_RESHAPE variable=output cyclic factor=8 dim=2
#pragma HLS ARRAY_RESHAPE variable=input cyclic factor=8 dim=2
#pragma HLS RESOURCE variable=output core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=input core=RAM_1P_BRAM
#pragma HLS INTERFACE bram port=output
#pragma HLS INTERFACE bram port=input
#pragma HLS INTERFACE s_axilite port=return bundle=control

	float inTile[N][N];
	float outTile[N][N];

	for (int tr = 0; tr < Tr; ++tr) {
		for (int tc = 0; tc < Tc; ++tc) {
#pragma HLS UNROLL factor=3
			for (int ir = 0; ir < N; ++ir) {
#pragma HLS UNROLL
				for (int ic = 0; ic < N; ++ic) {
#pragma HLS UNROLL
					inTile[ir][ic] = input[tr*N + ir][tc*N + ic];
					outTile[ir][ic] = 0.f;
				}
			}

			DCT_8x8(outTile, inTile);
			quantize_8x8(outTile);

			for (int ir = 0; ir < N; ++ir) {
#pragma HLS UNROLL
				for (int ic = 0; ic < N; ++ic) {
#pragma HLS UNROLL
					output[tr*N + ir][tc*N + ic] = outTile[ir][ic];
				}
			}
		}
	}
}
