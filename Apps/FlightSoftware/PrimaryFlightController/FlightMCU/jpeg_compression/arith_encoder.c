#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "arith_encoder.h"

int encoder(float** image_8x8, float* out_image){
  int i,j, len;
  int nz_cnt, len_del, row, col = 0;

  bool row_inc, is_num, is_zero = 0;

  for (len = 1; len <= N; ++len) { 
    for (i = 0; i < len; ++i) { 
      memcpy(out_image[nz_cnt], image_8x8[row][col], sizeof(float));
      printf("%f", image_8x8[row][col]);
      if(image_8x8[row][col] != 0)
        is_num = 1;

      if (i + 1 == len) 
        break; 
      if (row_inc) 
        ++row, --col; 
      else
        --row, ++col; 
    } 

    if(is_num == 0) {
      //dont go through second half of encoding
      //get rid of len num spaces at the end
      len_del = len;
      break;
    }
    else {
      is_num = 0;
      nz_cnt += len;
    } 

    if (len == N) 
      break; 
    if (row_inc) 
      ++row, row_inc = false; 
    else
      ++col, row_inc = true;
    
  } 

return nz_cnt;
}
/*
  if(len_del == 0){
    int M = N - 1; 
    for (int len = M; len > 0; --len) { 
      for (int i = 0; i < len; ++i) { 
        memcpy(out_image[nz_cnt], image_8x8[row][col], sizeof(float));
        if(image_8x8[row][col] != 0)
        is_num = 1;

        if (i + 1 == len) 
          break; 
        if (row_inc) 
          ++row, --col; 
        else
          ++col, --row; 
        } 
  
        // Update the indexes of row and col variable 
        if (row == 0 || col == M) { 
            if (col == m - 1) 
                ++row; 
            else
                ++col; 
  
            row_inc = true; 
        } 
  
        else if (col == 0 || row == M) { 
            if (row == n - 1) 
                ++col; 
            else
                ++row; 
  
            row_inc = false; 
        } 
    } 
  }
  */