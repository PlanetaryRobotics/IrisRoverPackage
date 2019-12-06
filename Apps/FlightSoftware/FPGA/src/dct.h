/**
 * Function declarations for DCT
 */

#ifndef DCT_H
#define DCT_H

#include <math.h>
#include <stdint.h>

#include "config.h"

#define N 8

#define Tr 1
#define Tc 324

#define Q 50.f
#define CUTOFF_IDX 64

#define NR (N*Tr)
#define NC (N*Tc)

#define ONE_BY_SQRT_TWO_N (0.25f)

inline float C(unsigned i) {
    return (i == 0) ? (0.7071067811865475) : 1.;
}

inline float dctCos(unsigned ij, unsigned xy) {
    return cos(((2.*xy + 1.) * ij * M_PI) / (2.*N));
}

void quantize_8x8(float image_8x8[N][N]);
void DCT_8x8(float output[8][8], const float input[8][8]);

void jpegCompress(int *output, const uint8_t *input, const int W, const int H);
void jpegCompressTiles(int output[NR][NC], const float input[NR][NC]);

#endif /* DCT_H */
