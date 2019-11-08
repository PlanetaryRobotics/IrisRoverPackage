#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//Transform Matrix

float** createArray(){
    float* values = calloc(N*N, sizeof(float));
    float** rows = malloc(N*sizeof(float*));
    int i;
    for (i = 0; i<N; ++i)
    {
        rows[i] = values + i*N;
    }
    return rows;
}

void destroyArray(float** tm){
    free(*tm);
    free(tm);
}

void transform_pointer(float** tm){
  int i,j;
  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++){
      if(i == 0){
        tm[i][j] = 1/sqrt(N);
      }
      else{
        tm[i][j] = (sqrt(2)/sqrt(N))*cosf(((2*j+1)*i*M_PI)/(2*N));
      } 
    }
  }
}

void invert_matrix(float** tm){
  int i;
  float determinant =0;
  for(i = 0; i < N; i++){
		determinant = determinant + (tm[0][i] * (tm[1][(i+1)%3] * tm[2][(i+2)%3] - tm[1][(i+2)%3] * tm[2][(i+1)%3]));
  }
  for(i = 0; i < N; i++){
    int j;
		for(j = 0; j < 3; j++)
			tm[i][j] = ((tm[(j+1)%3][(i+1)%3] * tm[(j+2)%3][(i+2)%3]) - (tm[(j+1)%3][(i+2)%3] * tm[(j+2)%3][(i+1)%3]))/ determinant;
	}
}

void mm_naive(float** R, float** M, float** T){
  unsigned long i , j, k;
  for (i = 0; i < N; i++){ 
    for (j = 0; j < N; j++){ 
      R[i][j] = 0; 
      for (k = 0; k < N; k++) 
        R[i][j] += T[i][k] *  M[k][j]; 
    } 
  } 
}

void dct(float** image_8x8, float** R, float** t_matrix, float** it_matrix){
  int i,j;
  for (i = 0; i < N; i++){ 
    for (j = 0; j < N; j++){
      image_8x8[i][j] = image_8x8[i][j] -128;
    }
  }
  image_8x8 = image_8x8 - 128;
  mm_naive(R, image_8x8, t_matrix);
  mm_naive(image_8x8, it_matrix, R);
}

/*void main(){
  float** t_matrix= createArray();
  transform_pointer(t_matrix);
  int i,j;
  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++){
      printf("%f  ", t_matrix[i][j]);
    }
    printf("\n"); 
  }
  destroyArray(t_matrix);
};*/
