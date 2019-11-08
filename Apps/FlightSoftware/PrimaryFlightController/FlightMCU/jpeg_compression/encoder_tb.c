#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "arith_encoder.h"


void enc_tb(){

  float* values = calloc(N*N, sizeof(float));
  float** rows = malloc(N*sizeof(float*));
  int i,j;
  for (i = 0; i<N; ++i)  
  {
    rows[i] = values + i*N;
  }

  for (i = 0; i<N; ++i) {
    for (j = 0; j<N; ++j) {
      rows[i][j] = (i*j) + 1;
    }
  }


  float* out_image = calloc(N*N, sizeof(float));
  int tot = encoder(rows,out_image);

  for (i = 0; i<N*N; ++i) {
    printf("%f ",out_image[i]);
  }

  free(*rows);
  free(rows);
  free(out_image);
}