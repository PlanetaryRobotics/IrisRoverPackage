/**
 * Run-length encoder for 8x8 image blocks
 */

#ifndef ENCODER_V2_H
#define ENCODER_V2_H

#define N 8

template <typename T>
int rle(const T image_8x8[8][8], T* out_image){
  int i, len;
  int cnt = 0;
  int zero_cnt = 0;
  int row = 0;
  int col = 0;

  bool row_inc = 0;

  for (len = 1; len <= N; ++len) {
    for (i = 0; i < len; ++i) {
      if(image_8x8[row][col] != 0){
        out_image[2*cnt] = zero_cnt;
        out_image[2*cnt + 1] = image_8x8[row][col];
        ++cnt,zero_cnt = 0;
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
      if(image_8x8[row][col] != 0){
        out_image[2*cnt] = zero_cnt;
        out_image[2*cnt + 1] = image_8x8[row][col];
        ++cnt,zero_cnt = 0;
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

  out_image[2*cnt] = 0;
  out_image[2*cnt + 1] = 0;
  cnt++;

  return 2*cnt;
}

#endif /* ENCODER_V2_H */
