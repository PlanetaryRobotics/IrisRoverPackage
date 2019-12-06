#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "decoder.h"
#include "dct.h"


const int q50 [N][N]=
  {{16, 11, 10, 16, 24, 40, 51, 61}, 
   {12, 12, 14, 19, 26, 58, 60, 55}, 
   {14, 13, 16, 24, 40, 57, 69, 56}, 
   {14, 17, 22, 29, 51, 87, 80, 62},
   {18, 22, 37, 56, 68, 109, 103, 77}, 
   {24, 35, 55, 64, 81, 104, 113, 92}, 
   {49, 64, 78, 87, 103, 121, 120, 101}, 
   {72, 92, 95, 98, 112, 100, 103, 99}};

int decoderv1(int* out_image, float image_8x8[8][8]){

	const float multiplier = (float)(100 - Q) / (float)50;

  int i, len;
  int cnt = 0;
  int zero_cnt = -1;
  int row = 0;
  int col = 0;

  bool row_inc = 0;

  for (len = 1; len <= N; ++len) { 
    for (i = 0; i < len; ++i) {
      if(out_image[cnt] == 0){
        if(out_image[cnt + 1] != 0){
          image_8x8[row][col] = out_image[cnt + 1] * q50[row][col] * multiplier;
          cnt+=2;
        }
        else
          image_8x8[row][col] = 0;
      }
      else if(out_image[cnt] != 0){
        if(zero_cnt == -1)
        {
          image_8x8[row][col] = 0;
          zero_cnt = out_image[cnt] - 1;
        }
        else if(zero_cnt == 0)
        {
          image_8x8[row][col] = out_image[cnt + 1] * q50[row][col] * multiplier;
          cnt+=2;
          zero_cnt--;
        }
        else{
          image_8x8[row][col] = 0;
          zero_cnt--;   
        }
      }
      if (i + 1 == len){
        break;
      }   
      if (row_inc) 
        ++row, --col; 
      else
        --row, ++col; 
    } 
    if (len == N){
      break;
    } 
    if (row_inc) 
      ++row, row_inc = false; 
    else
      ++col, row_inc = true;
  }

  if (row == 0) { 
        if (col == N - 1) 
            ++row; 
        else
            ++col; 
        row_inc = 1; 
  } 
  else { 
    if (row == N - 1) 
      ++col; 
    else
      ++row; 

    row_inc = 0; 
  }  

  int M = N - 1; 
  for (len = M; len > 0; --len) { 
    for (i = 0; i < len; ++i) { 
      if(out_image[cnt] == 0){
        if(out_image[cnt + 1] != 0){
          image_8x8[row][col] = out_image[cnt + 1] * q50[row][col] * multiplier;
          cnt+=2;
        }
        else
          image_8x8[row][col] = 0;
      }
      else if(out_image[cnt] != 0){
        if(zero_cnt == -1)
        {
          image_8x8[row][col] = 0;
          zero_cnt = out_image[cnt] - 1;
        }
        else if(zero_cnt == 0)
        {
          image_8x8[row][col] = out_image[cnt + 1] * q50[row][col] * multiplier;
          cnt+=2;
          zero_cnt--;
        }
        else{
          image_8x8[row][col] = 0;
          zero_cnt--;   
        }
      }
      if (i + 1 == len){    
        break;
      }   
      if (row_inc) 
        ++row, --col; 
      else
        ++col, --row; 
    }

    if (row == 0 || col == M) { 
      if (col == M) 
        ++row; 
      else
        ++col; 
        
      row_inc = true; 
    } 

    else if (col == 0 || row == M) { 
      if (row == M) 
        ++col; 
      else
        ++row; 
      row_inc = false; 
    }
  }
return (cnt + 2);
}
