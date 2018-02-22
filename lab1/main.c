#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "matrix.h"

#define STATIC_INIT 0
#define SHOW_MATRIX 0

// TODO: 
// + generate matrix of matrix
// * check ifndef about vectorization
// *  
//


int main(int argc, char *argv[]) {
	clock_t start_time, end_time;
	mtype **matrix1, ** matrix2;
	if (!STATIC_INIT) {
		matrix1 = matrix_init(false);
		matrix2 = matrix_init(false);
	} else {
		matrix1 = matric_static_init();
		matrix2 = matric_static_init();
	}
 
 	if (SHOW_MATRIX) {
		printf("%s\n", "First matrix:");
		matrix_show(matrix1);
		printf("%s\n", "Second matrix:");
		matrix_show(matrix2);
	}
	
	start_time = clock();
	mtype **c_result = matrix_multiply(matrix1, matrix2);
	end_time = clock();
	printf("C time in clocks: %ld\n", end_time - start_time);
	printf("C time in seconds: %f\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);
	if (SHOW_MATRIX) {
		printf("%s\n", "Result matrix:");
		matrix_show(c_result);
	}

	start_time = clock();
	mtype **asm_result = matrix_multiply_asm(matrix1, matrix2);
	end_time = clock();
	printf("asm time in clocks: %ld\n", end_time - start_time);
	printf("asm time in seconds: %f\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);

	matrix_compare(c_result, asm_result);
	
	matrix_destroy(matrix1);
	matrix_destroy(matrix2);
	matrix_destroy(c_result);
	matrix_destroy(asm_result);
	return 0;
}

