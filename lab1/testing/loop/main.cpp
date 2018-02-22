#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define M_HEIGHT	1
#define M_WIDTH 	1
#define M_ELEMENTS	(M_HEIGHT * M_WIDTH)
#define CELL_HEIGHT	64
#define CELL_WIDTH 	64
#define CELL_ELEMENTS	(CELL_HEIGHT * CELL_WIDTH)

#define SIZE	(1L << 16)

int** matric_static_init(int a)
{
	int **matrix = (int**)malloc(M_ELEMENTS * sizeof(int*));
	if (matrix == NULL) {
		perror("Matrix allocation");
		exit(1);
	}

	for (int i = 0; i < M_ELEMENTS; i++) {
		matrix[i] = (int*)malloc(CELL_ELEMENTS * sizeof(int));
		for (int j = 0; j < CELL_ELEMENTS; j++) {
			if (a > 0) {
				matrix[i][j] = i + j + 1;

			} else {
				matrix[i][j] = 0;
			}
		}
	}	
	return matrix;
}

void matrix_show(int **matrix)
{
	for (int i = 0; i < M_HEIGHT; i++) {
		for (int j = 0; j < M_WIDTH; j++) {
			printf("Matrix at [%d][%d]:\n", i, j);
			for (int k = 0; k < CELL_HEIGHT; k++) {
				for (int l = 0; l < CELL_WIDTH; l++) {
					printf("%16.1d", matrix[i * M_WIDTH + j][k * CELL_WIDTH + l]);
				}
				printf("\n");
			}
			printf("------------------");
			printf("------------------\n");
		}
	}
}


void matrix_multiply(int **x, int **y, int **z) 
{
	for (int q = 0; q < M_ELEMENTS; q++) {
		int *a = x[q];
		int *b = y[q];
		int *c = z[q];
		for (int i = 0; i < CELL_WIDTH; i++) {
			for (int j = 0; j < CELL_WIDTH; j++) {
				for (int k = 0; k < CELL_WIDTH; k++) {
					c[i*CELL_WIDTH + k] += b[j*CELL_WIDTH + k] * a[i*CELL_WIDTH + j];
				}
			}
		}
	}
}

int main() 
{
	int** a = matric_static_init(1);
	int** b = matric_static_init(1);
	int** c = matric_static_init(0);

	matrix_multiply(a, b, c);

	matrix_show(a);
	matrix_show(b);
	matrix_show(c);
	return 0;
}

// void test1(int **a, int **b, int **c)
// {
// 	for (int i = 0; i < M_ELEMENTS; i++) {
// 		int *x = a[i];
// 		int *y = b[i];
// 		for (int k = 0; k < CELL_HEIGHT; k++) {
// 			for (int m = 0; m < CELL_WIDTH; m++) {
// 				int res = 0;
// 				for (int j = 0; j < SIZE; j++) {
// 					res += x[j] + y[j];
// 				}
// 				c[i][k] = res;
// 			}
// 		}
// 	}
// }

// int** matrix_multiply(int ** first_matrix, int ** second_matrix)
// {
// 	int** result;
// 	for (int i = 0; i < M_ELEMENTS; i++) {
// 		int *a = first_matrix[i];
// 		int *b = second_matrix[i];
// 		for (int k = 0; k < CELL_HEIGHT; k++) {
// 			for (int m = 0; m < CELL_WIDTH; m++) {
// 				int res_el = 0;
// 				for (int l = 0; l < CELL_WIDTH; l++) {
// 					res_el += (a[k * CELL_WIDTH + l] * b[l * CELL_WIDTH + m]);
// 				}
// 				result[i][k * CELL_HEIGHT + m] = res_el;
// 				printf("%d\n", res_el);
// 			}
// 		}

// 	}
// 	return result;
// }