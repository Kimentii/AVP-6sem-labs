#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

#include "matrix.h"

static long int random_seed = 0;

mtype* matrix_init(bool zero) 
{	
	mtype *matrix = (mtype*)malloc(M_ELEMENTS * sizeof(mtype));
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
	
	if (zero) {
		for (int i = 0; i < M_ELEMENTS; i++) {
			matrix[i] = 0;	
		}	
	} else {
		for (int i = 0; i < M_ELEMENTS; i++) {
			matrix[i] = mrand48() + drand48();		
		}	
	}
	return matrix;
}

mtype* matric_static_init()
{
	mtype *matrix = (mtype*)malloc(M_ELEMENTS * sizeof(mtype));
	if (matrix == NULL) {
		perror("Matrix allocation");
		exit(EXIT_ERROR);
	}

	for (int i = 0; i < M_ELEMENTS; i++) {
		matrix[i] = i + 1;
	}	
	return matrix;
}

void matrix_show(mtype *matrix)
{
	for (int i = 0; i < M_HEIGHT; i++) {
		for (int j = 0; j < M_WIDTH; j++) {
			printf("%16.1F", matrix[i * M_WIDTH + j]);
		}
		printf("\n");
	}
}

mtype* matrix_multiply(mtype *first_matrix, mtype *second_matrix)
{
	// omp_set_num_threads(4); //initialise thread count for 4 core cpu    

	mtype* result = matrix_init(true);
	#pragma omp parallel for
	for (int i = 0; i < M_HEIGHT; i++) {
		for (int j = 0; j < M_WIDTH; j++) {
			
			for (int k = 0; k < M_WIDTH; k++) {
				// printf(" %d", omp_get_thread_num());
				// printf("i[%d], j[%d], k[%d] = %ld\n", i, j, k, b[j*CELL_WIDTH + k] * a[i*CELL_WIDTH + j]);
				result[i*M_WIDTH + j] += first_matrix[i*M_WIDTH + k] * second_matrix[k*M_WIDTH + j];
			}
		}
	}
	return result;
}

mtype* matrix_multiply_cache(mtype* a, mtype* b)
{
	mtype* result = matrix_init(true);
	for(int aby = 0; aby < B_IN_HEIGHT; aby++) {			// a block 'y'
		for(int abx = 0; abx < B_IN_WIDTH; abx++) {		// a block 'x'
			for(int bbx = 0; bbx < B_IN_WIDTH; bbx++) {	// b block 'x'
//				int bby = abx;				// b block 'y'
//				int rby = aby;
//				int rbx = bbx;
				for (int ryc = 0; ryc < B_HEIGHT; ryc++) {
					for (int rxc = 0; rxc < B_WIDTH; rxc++) {
						//int rp = ((aby * B_HEIGHT) + ryc) * M_WIDTH + ((bbx * B_WIDTH) + rxc);
						for (int axc = 0; axc < B_WIDTH; axc++) {		// a block 'y' counter 
							//int ap = ((aby * B_HEIGHT) + ryc) * M_WIDTH + ((abx * B_WIDTH) + axc);
							//int bp = ((bby * B_HEIGHT) + axc) * M_WIDTH + ((bbx * B_WIDTH) + rxc);
//int bxc = ayc;					
// b block 'x' counter
//							for (int axc = 0; axc < B_WIDTH; axc++) {	// a block 'x' counter
//								int byc = axc;				// b block 'y' counter
//								int ap = ((aby * B_HEIGHT) + ayc) * M_WIDTH + ((abx * B_WIDTH) + axc);
//								int bp = ((bby * B_HEIGHT) + byc) * M_WIDTH + ((bbx * B_WIDTH) + bxc);
//								printf("abx: %d; aby: %d, bbx: %d, bby: %d, ap: %d; bp: %d\n", abx, aby, bbx, bby, ap, bp);
								//result[rp] += a[ap] * b[bp];
								result[((aby * B_HEIGHT) + ryc) * M_WIDTH + ((bbx * B_WIDTH) + rxc)] += a[((aby * B_HEIGHT) + ryc) * M_WIDTH + ((abx * B_WIDTH) + axc)] * b[((abx * B_HEIGHT) + axc) * M_WIDTH + ((bbx * B_WIDTH) + rxc)];
//							}
						}
					}
				}
			
			}
		}
	}
	return result;
}
			
//int bbx = aby;
//			int bby = 			
//for (i = 	// selecting block to multiply on
//			// multipy inner blocks here
//			int result_block_row = 
//			for(int rh = 0; rh < B_HEIGHT; rh++) {
//				for(int rw = 0; rw < B_WIDTH; rw++) {
//					for(int h 
//					result[] += a * b
//					for(int j = m * B_WIDTH; j < (m + 1) * B_WIDTH; j++) {
//						for(int k = n * B_WIDTH; k < (n + 1) * B_WIDTH; k++) {
//							result[i * M_WIDTH+ j] += first_matrix[i * M_WIDTH + k] *
//							 second_matrix[k * M_WIDTH + j];
//						}
//					}
//				}
//			}
//		}
//	}
//	/*for (int i = 0; i < M_WIDTH; i++) {
//		for (int j = 0; j < M_HEIGHT; j++) {
//
//		}
//	}*/
//	return result;
//}

void matrix_compare(mtype* a, mtype* b) 
{
	for (int i = 0; i < M_ELEMENTS; i++) {
		assert(a[i] == b[i]);
	}
}

void matrix_destroy(mtype *matrix)
{
	free(matrix);
}


