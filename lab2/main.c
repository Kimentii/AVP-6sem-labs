#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>

#include "matrix.h"

#define STATIC_INIT 0
#define SHOW_MATRIX 0

void show_usage_exit() {
	printf("Usage: \n\tlab2 [par]\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	clock_t start_time, end_time;
	mtype *matrix1, *matrix2, *c_result, *cache_result;
	if (!STATIC_INIT) {
		matrix1 = matrix_init(false);
		matrix2 = matrix_init(false);
	} else {
		matrix1 = matric_static_init();
		matrix2 = matric_static_init();
	}
 
	if (argc < 2) {	
		start_time = clock();
		c_result = matrix_multiply(matrix1, matrix2);
		end_time = clock();
		printf("Time in clocks: %ld\n", end_time - start_time);
		printf("Time in seconds: %f\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);

		start_time = clock();
		cache_result = matrix_multiply_cache(matrix1, matrix2);
		end_time = clock();
		printf("Cache time in clocks: %ld\n", end_time - start_time);
		printf("Cache time in seconds: %f\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);
		matrix_destroy(c_result);
		matrix_destroy(cache_result);
	} else if (strcmp("cache", argv[1]) == 0) {
		printf("Using cache...\n");
		start_time = clock();
		cache_result = matrix_multiply_cache(matrix1, matrix2);
		end_time = clock();
		printf("Cache time in clocks: %ld\n", end_time - start_time);
		printf("Cache time in seconds: %f\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);
		if (SHOW_MATRIX) {
			printf("%s\n", "Result matrix:");
			matrix_show(cache_result);
		}
		matrix_destroy(cache_result);
	} else if (strcmp("no-cache", argv[1]) == 0) {				
		start_time = clock();
		c_result = matrix_multiply(matrix1, matrix2);
		end_time = clock();
		printf("Time in clocks: %ld\n", end_time - start_time);
		printf("Time in seconds: %f\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);
		if (SHOW_MATRIX) {
			printf("%s\n", "Result matrix:");
			matrix_show(c_result);
		}
		matrix_destroy(c_result);
	}
	
	matrix_destroy(matrix1);
	matrix_destroy(matrix2);
	return 0;
}
