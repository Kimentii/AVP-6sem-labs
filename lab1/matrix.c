#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "matrix.h"

static long int random_seed = 0;

mtype** matrix_init(bool zero) 
{	
	mtype **matrix = (mtype**)malloc(M_ELEMENTS * sizeof(mtype*));
	if (matrix == NULL) {
		perror("Matrix allocation");
		exit(EXIT_ERROR);
	}
	if (random_seed == 0) {
		FILE* random_file = fopen("/dev/urandom", "return");
		if (random_file == NULL) {
			perror("Open urandom");
			exit(EXIT_ERROR);
		}

		if (fread(&random_seed, sizeof(long int), 1, random_file) < 1){
			perror("Read urandom");
			exit(EXIT_ERROR);
		}
		fclose(random_file);

		srand48(random_seed);
	}
	

	for (int i = 0; i < M_ELEMENTS; i++) {
		matrix[i] = (mtype*)malloc(CELL_ELEMENTS * sizeof(mtype));
		if (zero) {
			for (int j = 0; j < CELL_ELEMENTS; j++) {
				matrix[i][j] = 0;
			}
		} else {
			for (int j = 0; j < CELL_ELEMENTS; j++) {
				matrix[i][j] = mrand48() + drand48();
			}			
		}
	}	
	return matrix;
}

mtype** matric_static_init()
{
	mtype **matrix = (mtype**)malloc(M_ELEMENTS * sizeof(mtype*));
	if (matrix == NULL) {
		perror("Matrix allocation");
		exit(EXIT_ERROR);
	}

	for (int i = 0; i < M_ELEMENTS; i++) {
		matrix[i] = (mtype*)malloc(CELL_ELEMENTS * sizeof(mtype));
		for (int j = 0; j < CELL_ELEMENTS; j++) {
			matrix[i][j] = i + j + 1;
		}
	}	
	return matrix;
}
void matrix_show(mtype **matrix)
{
	for (int i = 0; i < M_HEIGHT; i++) {
		for (int j = 0; j < M_WIDTH; j++) {
			printf("Matrix at [%d][%d]:\n", i, j);
			for (int k = 0; k < CELL_HEIGHT; k++) {
				for (int l = 0; l < CELL_WIDTH; l++) {
					printf("%16.1F", matrix[i * M_WIDTH + j][k * CELL_WIDTH + l]);
				}
				printf("\n");
			}
			printf("------------------------------------\n");
		}
	}
}

mtype** matrix_multiply(mtype ** first_matrix, mtype ** second_matrix)
{

	mtype** result = matrix_init(true);
	for (int i = 0; i < M_ELEMENTS; i++) {
		mtype *a = first_matrix[i];
		mtype *b = second_matrix[i];
		mtype *c = result[i];
		for (int i = 0; i < CELL_HEIGHT; i++) {
			for (int j = 0; j < CELL_WIDTH; j++) {
				for (int k = 0; k < CELL_HEIGHT; k++) {
					// printf("i[%d], j[%d], k[%d] = %ld\n", i, j, k, b[j*CELL_WIDTH + k] * a[i*CELL_WIDTH + j]);
					c[i*CELL_HEIGHT + k] += b[j*CELL_WIDTH + k] * a[i*CELL_HEIGHT + j];
				}
			}
		}
	}
	return result;
}

mtype** matrix_multiply_asm(mtype ** first_matrix, mtype ** second_matrix)
{
	mtype** result = matrix_init(true);
	for (int i = 0; i < M_ELEMENTS; i++) {
		mtype *a = first_matrix[i];
		mtype *b = second_matrix[i];
		mtype *c = result[i];
		for (int i = 0; i < CELL_HEIGHT; i++) {
			for (int j = 0; j < CELL_WIDTH; j++) {
				for (int k = 0; k < CELL_HEIGHT; k+=2) {
					__asm volatile("movupd (%1), %%xmm0; \
						movddup (%2), %%xmm1; \
						mulpd %%xmm1, %%xmm0; \
						movups (%0), %%xmm1; \
						addpd %%xmm1, %%xmm0; \
						movups %%xmm0, (%0)"
						: 
						: "r"(c + (i*CELL_HEIGHT + k)), "r"(b + (j*CELL_WIDTH + k)), "r"(a + (i*CELL_HEIGHT + j))
						: "%xmm0", "%xmm1");
				}
			}
		}
	}
	return result;
}

void matrix_compare(mtype** a, mtype**b) 
{
	for (int i = 0; i < M_ELEMENTS; i++) {
		for (int j = 0; j < CELL_ELEMENTS; j++) {
			assert(a[i][j] == b[i][j]);
		}
	}
}

void matrix_destroy(mtype **matrix)
{
	for(int i = 0; i < M_ELEMENTS; i++) {
		free(matrix[i]);
	}
	free(matrix);
}


