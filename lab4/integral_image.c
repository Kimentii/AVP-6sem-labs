#include "integral_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define L 100

void fill_random(int *src, int length) {
	time_t t;
	time(&t);
	srand((unsigned int)t);
	for (int i = 0; i < length; i++) {
		src[i] = rand() % L;
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
			result[i*n + j] = result[(i-1)*n + j] + result[(i*n) + j-1] 
						- result[(i-1)*n + j-1] + src[(i*n) + j];
		}
	}
}

void integral_image_with_secret(int *src, int *result, int n, int m) 
{
	int sum = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			sum = 0;
			for (int l = 0; l <= i; l++) {
				for (int k = 0; k <= j; k++) {
					sum += src[l * n + k];
				}
			}
			result[i * n + j] = sum;
		}
	}
}
void print_matrix(int *m, int width, int length) {
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < length; j++) {
			printf("%5d ", m[(i*width) + j]);
		}
		printf("\n");
	} 
}

void matrix_compare(int* a, int* b, int size)
{
	for (int i = 0; i < size; i++) {
		assert(a[i] == b[i]);
	}
}