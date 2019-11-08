#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "dct.h"
#include "quantization.h"

//dct
//quantize
//encode

void main(float** input_image){
  float** t_matrix = createArray();
  transform_pointer(t_matrix);

  float** it_matrix = createArray();
  memcpy(it_matrix, t_matrix, N*N*sizeof(float));
  invert_matrix(it_matrix);

  float** R = createArray();
  //for X amount of N 8x8 blocks
    void dct(image_8x8, R, t_matrix, it_matrix);
    void quantization(image_8x8);
    float* out_image = calloc(N*N, sizeof(float));
    void encoder(image_8x8, out_image);
  
  free(zero_check);
  destroyArray(R);



}