#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32) || defined(WIN64)
#include <conio.h>
#endif
#include "filter.h"
#include "cl_wrapper.h"
#include "pgm.h"
#pragma comment(lib, "OpenCL.lib")

const char KERNEL_FUN[] = "kernel_fun";
const char KERNEL_FILE_NAME[] = "kernel.cl";
const char INPUT_IMG_NAME[] = "image.pgm";
const char OUTPUT_IMG_NAME[] = "out_image.pgm";
const int NUM_OF_DIMENTIONS = 2;
const int BLOCK_WIDTH = 256;
const int BLOCK_HEIGHT = 1;
const int FILTER_SIZE = 21;

int main()
{
	PGMImage* image = read_pgm(INPUT_IMG_NAME);
	add_frame(image, 5);
	resize_image(image, BLOCK_WIDTH, BLOCK_HEIGHT);
	write_pgm_with_frame(image, OUTPUT_IMG_NAME);
	/*ftype* filter = get_filter(FILTER_SIZE, FILTER_SIZE, FILTER_TYPE_BLUR);
	normalize_filter(filter, FILTER_SIZE, FILTER_SIZE, 1);
	matrix_show(filter, 3, 3);
	PGMImage* image = read_pgm(INPUT_IMG_NAME);
	add_frame(image, FILTER_SIZE / 2);
	resize_image(image, BLOCK_WIDTH, BLOCK_HEIGHT);

	PGMImage* output_image = filter_via_cpu(image, filter, FILTER_SIZE);

	printf("Input image:\n");
	show_iamge_part(image, 5, 5);
	printf("\n");
	printf("Output image:\n");
	show_iamge_part(output_image, 5, 5);
	printf("\n");
	write_pgm(output_image, OUTPUT_IMG_NAME);
	printf("finished.");
	*/
	printf("End");
#if defined(WIN32) || defined(WIN64)
	_getch();
#endif
	return 0;
}

