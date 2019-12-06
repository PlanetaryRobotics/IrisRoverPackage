#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <vector>

#include "encoder_v2cpp.h"

using namespace std;

void encoderv2( float image_8x8[N][N], std::vector<int8_t> &out_image) {
  int i,j, len;
  int zero_cnt = 0;
  int row = 0;
  int col = 0;

  bool row_inc = 0;

  for (len = 1; len <= N; ++len) { 
    for (i = 0; i < len; ++i) {
      if(image_8x8[row][col] !=0){
        out_image.push_back(zero_cnt);
        out_image.push_back((int8_t)image_8x8[row][col]);
        zero_cnt = 0;
      }
      else
        ++zero_cnt;
      
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
      if(image_8x8[row][col] !=0){
        out_image.push_back(zero_cnt);
        out_image.push_back((int8_t)image_8x8[row][col]);
        zero_cnt = 0;
      }
      else
        ++zero_cnt;
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
  if(zero_cnt == 64){
    out_image.push_back(0);
    out_image.push_back(0);
  }
}
