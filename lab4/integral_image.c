#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6
#define L 100

void fill_random(int *src, int length) {
	time_t t;
	time(&t);
	srand((unsigned int)t);
	for (int i = 0; i < length; i++) {
		src[i] = (int)random() % L;
	}
}

void integral_image(int *src, int *result, int n, int m)
{
	result[0] = src[0];
	for(int i = 1; i < n; i++) {
		result[i] = result[i-1] + src[i];
	}
	for(int i = 1; i < n; i++) {
		result[n*i] = result[n*(i-1)] + src[n*i];
	}
	for(int i = 1; i < n; i++) {
		for(int j = 1; j < m; j++) {
			result[i*N + j] = result[(i-1)*N + j] + result[(i*N) + j-1] 
						- result[(i-1)*N + j-1] + src[(i*N) + j];
		}
	} 
}

void p_m(int *m) {
	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			printf("%d ", m[(i*N) + j]); 
		}
		printf("\n");
	} 
}

int main() {
	int *matrix = (int*)malloc(N*N*sizeof(int));
	int *matrix2 = (int*)malloc(N*N*sizeof(int));
	fill_random(matrix, N*N);
	integral_image(matrix, matrix2, N, N);
	p_m(matrix);
	printf("---\n");
	p_m(matrix2);
	return 0;
}
