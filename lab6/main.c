#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cl_wrapper.h"
#include "cl_api.h"
#include "pgm.h"
#include "matrix.h"
#include "filter.h"

#define WAVEFRONT_SIZE = 64
#define GLOBAL_BANK_ROW_SIZE = 256;

#define KERNEL_FILE_NAME = "kernel.cl";
#define KERNEL_FUN = "kernel_fun";
#define INPUT_IMG_NAME = "monkey.pgm";
#define OUTPUT_IMG_NAME = "out_image.pgm";

#define FILTER_SIZE	3

int main()
{
	ftype* filter = get_filter(FILTER_SIZE, FILTER_SIZE, FILTER_TYPE_BLUR);
	normalize_filter(filter, FILTER_SIZE, FILTER_SIZE, 1);
	matrix_show(filter, FILTER_SIZE, FILTER_SIZE);
//	cl_platform_id platform_id = get_platform_id();
//	if (call_errno != CALL_SUCCESS) {
//		goto exit;
//	}
//
//	cl_device_id device_id = get_device_id(platform_id);
//	if (call_errno != CALL_SUCCESS) {
//		goto exit;
//	}
//
//	cl_context context = clCreateContext(
//				NULL, 1, &device_id, NULL, NULL, &cl_errno
//	);
//	if (cl_errno != CL_SUCCESS) {
//		printf("Error during creating context: %d\n", cl_errno);
//		goto exit;
//	}
//
//	cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, NULL);
//	if (cl_errno != CL_SUCCESS) {
//		printf("Error during creating command queue: %d\n", cl_errno);
//		goto context_release;
//	}
//	
//	//mtype *source_matrix = matrix_create(MATRIX_WIDTH, MATRIX_HEIGHT);
//
//	cl_mem matrix_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, MATRIX_SIZE_BYTES, source_matrix, &cl_errno);
//	if (cl_errno != CL_SUCCESS) {
//		printf("Error during buffer allocation\n");
//		goto matrix_release;
//	}
//	
//	char group_size_define[32];
//	if (sprintf(group_size_define, "-DGROUP_SIZE=%ld", group_size) < 0) {
//		printf("Can not set group size define for OpenCL program, exit\n");
//		goto matrix_release;
//	} else {
//		//printf("Adding define: %s\n", group_size_define);
//	}
//
//	cl_program program = get_cl_program(context, "int_img.cl");
//	cl_errno = clBuildProgram(program, 1, &device_id, group_size_define, NULL, NULL);
//	if (cl_errno != CL_SUCCESS) {
//		printf("Error during building program: %d\n", cl_errno);
//		exit(0);
//	}
//
//	size_t matrix_height = MATRIX_HEIGHT;
//	size_t matrix_width = MATRIX_WIDTH;
//
//	long int vertical_invocations = matrix_height / group_size;
//	if (matrix_height % group_size != 0) vertical_invocations++;
//
//	const size_t vertical_task_size[2] = 	{MATRIX_WIDTH,	group_size};
//	const size_t vertical_group_size[2] = {1, 						group_size};
//	const size_t horizontal_task_size[2] = 	{group_size, MATRIX_HEIGHT};
//	const size_t horizontal_group_size[2] = {group_size, 1};
//
//		cl_kernel vertical_kernel = clCreateKernel(program, "image_vertical", NULL);
//		clSetKernelArg(vertical_kernel, 0, sizeof(cl_mem), (void*)&matrix_buffer);
//		clSetKernelArg(vertical_kernel, 1, sizeof(long int), (void*)&matrix_height);
//		clSetKernelArg(vertical_kernel, 2, sizeof(long int), (void*)&matrix_width);
//
//		cl_kernel horizontal_kernel = clCreateKernel(program, "image_horizontal", NULL);
//		clSetKernelArg(horizontal_kernel, 0, sizeof(cl_mem), (void*)&matrix_buffer);
//		clSetKernelArg(horizontal_kernel, 1, sizeof(long int), (void*)&matrix_height);
//		clSetKernelArg(horizontal_kernel, 2, sizeof(long int), (void*)&matrix_width);
//
//	long long cl_time_start = rdtsc();
//	for (int i = 0, h = matrix_height; h > 0; i++, h-=group_size) {
//		//printf("--- %d ---\n", i);
//		//printf("Time before set arg: \t%lld\n", rdtsc() - cl_time_start);
//		clSetKernelArg(vertical_kernel, 3, sizeof(int), (void*)&i);
//		//printf("Time after set arg: \t%lld\n", rdtsc() - cl_time_start);
//		
//		clEnqueueNDRangeKernel(
//								queue, 
//								vertical_kernel, 
//								2, 
//								0, 
//								vertical_task_size, 
//								vertical_group_size, 
//								0, 
//								NULL, 
//								NULL
//		);
//	}
//
//	for (int i = 0, w = matrix_width; w > 0; i++, w-=group_size) {
//		//printf("Time: %lld\n", rdtsc() - cl_time_start);
//		clSetKernelArg(horizontal_kernel, 3, sizeof(int), (void*)&i);
//		
//		clEnqueueNDRangeKernel(
//								queue, 
//								horizontal_kernel, 
//								2, 
//								0, 
//								horizontal_task_size, 
//								horizontal_group_size, 
//								0, 
//								NULL, 
//								NULL
//		);
//	}
//
//	mtype result_matrix[MATRIX_WIDTH * MATRIX_HEIGHT];
//	cl_errno = clEnqueueReadBuffer(
//									queue, 
//									matrix_buffer, 
//									CL_BLOCKING, 
//									0, 
//									MATRIX_SIZE_BYTES, 
//									result_matrix, 
//									0,
//									NULL, 
//									NULL
//	);
//	if (cl_errno != CL_SUCCESS) {
//		printf("Error during reading result matrix: %d\n", cl_errno);
//		goto matrix_release;
//	}
//	clFinish(queue);
//	long long cl_time = rdtsc() - cl_time_start;
//	printf("====\n");
//
//	long long c_time_start = rdtsc();
//	mtype* c_result = matrix_int_img(source_matrix, MATRIX_HEIGHT, MATRIX_WIDTH);
//	long long c_time = rdtsc() - c_time_start;
//
//	if(memcmp(c_result, result_matrix, MATRIX_SIZE_BYTES) == 0) {
//		printf("Validation success!\n");
//	}	else {
//		printf("Validation failed!\n");
//	}
//		printf("CL time: \t%lld\n", cl_time);
//		printf("C time: \t%lld\n", c_time);
//	//matrix_show(result_matrix, MATRIX_HEIGHT, MATRIX_WIDTH);
//	//printf("=== C ===\n");
//	//matrix_show(c_result, MATRIX_HEIGHT, MATRIX_WIDTH);
//
//cl_program_release:
//	clReleaseProgram(program);
//cl_buffer_release:
//	clReleaseMemObject(matrix_buffer);
//matrix_release:
//	matrix_release(source_matrix);
//queue_release:
//	clReleaseCommandQueue(queue);
//context_release:
//	clReleaseContext(context);
//exit:
//	return 0;
//
//
//
//
//
//	cl_device_id device_id = get_device_id();
//	cl_context context = get_context(device_id);
//	cl_command_queue command_queue = get_command_queue(context, device_id);
//	cl_program program = create_program(context, device_id, KERNEL_FILE_NAME);
//	cl_kernel kernel = create_kernel(KERNEL_FUN, program);
//	
//	cl_mem input_image_obj;
//	cl_mem output_image_obj;
//
//	cl_int ret;
//	PGMImage* input_image = read_pgm(INPUT_IMG_NAME);
//	printf("Input image, sizeX: %d, sizeY: %d\n", input_image->sizeX, input_image->sizeY);
//	int width_in_blocks = input_image->sizeX / BLOCK_WIDTH;
//	if (!(width_in_blocks * BLOCK_WIDTH == input_image->sizeX)) {
//		width_in_blocks++;
//	}
//	int height_in_blocks = input_image->sizeY / BLOCK_HEIGHT;
//	if (!(height_in_blocks * BLOCK_HEIGHT == input_image->sizeY)) {
//		height_in_blocks++;
//	}
//
//	printf("width_in_blocks: %d, height_in_blocks: %d\n", width_in_blocks, height_in_blocks);
//
//	int required_matrix_width = width_in_blocks * BLOCK_WIDTH;
//	int required_matrix_heigth = height_in_blocks * BLOCK_HEIGHT;
//
//	printf("required_matrix_width: %d, required_matrix_heigth: %d\n", 
//		required_matrix_width, required_matrix_heigth);
//
//	PGMImage new_image;
//	new_image.data = (unsigned char*)calloc(required_matrix_width 
//		* required_matrix_heigth, sizeof(unsigned char));
//	for (int i = 0; i < required_matrix_heigth; i++) {
//		for (int j = 0; j < required_matrix_width; j++) {
//			new_image.data[i*required_matrix_width + j] = 10;
//		}
//	}
//
//	new_image.sizeX = required_matrix_width;
//	new_image.sizeY = required_matrix_heigth;
//
//	for (int i = 0; i < input_image->sizeY; i++) {
//		for (int j = 0; j < input_image->sizeX; j++) {
//			new_image.data[i*required_matrix_width + j] =
//				input_image->data[i*input_image->sizeX + j];
//		}
//	}
//
//	write_pgm(&new_image, OUTPUT_IMG_NAME);
//	/*
//	input_image_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, input_image->sizeX * input_image->sizeY * sizeof(char), NULL, &ret);
//	show_error(ret, "clCreateBuffer");
//	output_image_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, input_image->sizeX * input_image->sizeY * sizeof(char), NULL, &ret);
//	show_error(ret, "clCreateBuffer");
//	
//	ret = clEnqueueWriteBuffer(command_queue, input_image_obj, CL_TRUE, 0, input_image->sizeX * input_image->sizeY * sizeof(char),
//		input_image->data, 0, NULL, NULL);
//	show_error(ret, "clEnqueueWriteBuffer");
//	
//	clSetKernelArg(kernel, 0, sizeof(input_image_obj), (void*)&input_image_obj);
//	clSetKernelArg(kernel, 1, sizeof(output_image_obj), (void*)&output_image_obj);
//	clSetKernelArg(kernel, 2, sizeof(cl_int), &(input_image->sizeX));
//	
//	size_t global_work_size[] = { input_image->sizeX, input_image->sizeY };
//	ret = clEnqueueNDRangeKernel(command_queue, kernel, NUM_OF_DIMENTIONS, NULL, global_work_size, NULL, 0, NULL, NULL);
//	clFinish(command_queue);
//	
//	PGMImage output_image;
//	output_image.data = malloc(sizeof(char) * input_image->sizeX * input_image->sizeY);
//	output_image.sizeX = input_image->sizeX;
//	output_image.sizeY = input_image->sizeY;
//	ret = clEnqueueReadBuffer(command_queue, output_image_obj, CL_TRUE, 0, input_image->sizeX * input_image->sizeY * sizeof(char),
//		output_image.data, 0, NULL, NULL);
//	show_error(ret, "clEnqueueReadBuffer");
//	
//	write_pgm(&output_image, OUTPUT_IMG_NAME);
//
//	*/
//	printf("Kernels finished.");
//	#if defined(WIN32) || defined(WIN64)
//	_getch();
//	#endif
	return 0;
}

