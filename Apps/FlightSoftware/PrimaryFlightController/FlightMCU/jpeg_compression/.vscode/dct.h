//dct_tranform.h

#define N 8

float** createArray();
void destroyArray(float** tm);
void dct(float** image_8x8, float** R, float** t_matrix, float** it_matrix);
