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

	printf("Kernels finished.");
	#if defined(WIN32) || defined(WIN64)
	_getch();
	#endif
	return 0;
}

