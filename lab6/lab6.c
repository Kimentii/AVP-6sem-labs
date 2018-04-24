#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32) || defined(WIN64)
#include <conio.h>
#endif
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

int main()
{
	cl_device_id device_id = get_device_id();
	cl_context context = get_context(device_id);
	cl_command_queue command_queue = get_command_queue(context, device_id);
	cl_program program = create_program(context, device_id, KERNEL_FILE_NAME);
	cl_kernel kernel = create_kernel(KERNEL_FUN, program);
	
	cl_mem input_image_obj;
	cl_mem output_image_obj;

	cl_int ret;
	PGMImage* input_image = read_pgm(INPUT_IMG_NAME);
	printf("Input image, sizeX: %d, sizeY: %d\n", input_image->sizeX, input_image->sizeY);
	int width_in_blocks = input_image->sizeX / BLOCK_WIDTH;
	if (!(width_in_blocks * BLOCK_WIDTH == input_image->sizeX)) {
		width_in_blocks++;
	}
	int height_in_blocks = input_image->sizeY / BLOCK_HEIGHT;
	if (!(height_in_blocks * BLOCK_HEIGHT == input_image->sizeY)) {
		height_in_blocks++;
	}

	printf("width_in_blocks: %d, height_in_blocks: %d\n", width_in_blocks, height_in_blocks);

	int required_matrix_width = width_in_blocks * BLOCK_WIDTH;
	int required_matrix_heigth = height_in_blocks * BLOCK_HEIGHT;

	printf("required_matrix_width: %d, required_matrix_heigth: %d\n", 
		required_matrix_width, required_matrix_heigth);

	PGMImage new_image;
	new_image.data = (unsigned char*)calloc(required_matrix_width 
		* required_matrix_heigth, sizeof(unsigned char));
	for (int i = 0; i < required_matrix_heigth; i++) {
		for (int j = 0; j < required_matrix_width; j++) {
			new_image.data[i*required_matrix_width + j] = 10;
		}
	}

	new_image.sizeX = required_matrix_width;
	new_image.sizeY = required_matrix_heigth;

	for (int i = 0; i < input_image->sizeY; i++) {
		for (int j = 0; j < input_image->sizeX; j++) {
			new_image.data[i*required_matrix_width + j] =
				input_image->data[i*input_image->sizeX + j];
		}
	}

	write_pgm(&new_image, OUTPUT_IMG_NAME);
	/*
	input_image_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, input_image->sizeX * input_image->sizeY * sizeof(char), NULL, &ret);
	show_error(ret, "clCreateBuffer");
	output_image_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, input_image->sizeX * input_image->sizeY * sizeof(char), NULL, &ret);
	show_error(ret, "clCreateBuffer");
	
	ret = clEnqueueWriteBuffer(command_queue, input_image_obj, CL_TRUE, 0, input_image->sizeX * input_image->sizeY * sizeof(char),
		input_image->data, 0, NULL, NULL);
	show_error(ret, "clEnqueueWriteBuffer");
	
	clSetKernelArg(kernel, 0, sizeof(input_image_obj), (void*)&input_image_obj);
	clSetKernelArg(kernel, 1, sizeof(output_image_obj), (void*)&output_image_obj);
	clSetKernelArg(kernel, 2, sizeof(cl_int), &(input_image->sizeX));
	
	size_t global_work_size[] = { input_image->sizeX, input_image->sizeY };
	ret = clEnqueueNDRangeKernel(command_queue, kernel, NUM_OF_DIMENTIONS, NULL, global_work_size, NULL, 0, NULL, NULL);
	clFinish(command_queue);
	
	PGMImage output_image;
	output_image.data = malloc(sizeof(char) * input_image->sizeX * input_image->sizeY);
	output_image.sizeX = input_image->sizeX;
	output_image.sizeY = input_image->sizeY;
	ret = clEnqueueReadBuffer(command_queue, output_image_obj, CL_TRUE, 0, input_image->sizeX * input_image->sizeY * sizeof(char),
		output_image.data, 0, NULL, NULL);
	show_error(ret, "clEnqueueReadBuffer");
	
	write_pgm(&output_image, OUTPUT_IMG_NAME);

	*/
	printf("Kernels finished.");
	#if defined(WIN32) || defined(WIN64)
	_getch();
	#endif
	return 0;
}

