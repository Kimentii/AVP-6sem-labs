#include <CL/cl.h>
#include <stdio.h>
#include "cl_api.h"
#include "pgm.h"
#include "matrix.h"
#include "filter.h"

#define WAVEFRONT_SIZE 64
#define GLOBAL_BANK_ROW_SIZE 256

#define KERNEL_FILE_NAME "kernel.cl"
#define KERNEL_FUN "kernel_fun"
#define INPUT_IMG_NAME "monkey.pgm"
#define OUTPUT_IMG_NAME "out_image.pgm"

int main()
{
	int filter_size = 3;
	ftype* filter = get_filter(filter_size, filter_size, FILTER_TYPE_BLUR);
	normalize_filter(filter, filter_size, filter_size, 1);
	matrix_show(filter, filter_size, filter_size);


	cl_platform_id platform_id = get_platform_id();
	if (call_errno != CALL_SUCCESS) {
		goto exit;
	}

	cl_device_id device_id = get_device_id(platform_id);
	if (call_errno != CALL_SUCCESS) {
		goto exit;
	}

	cl_context context = clCreateContext(
				NULL, 1, &device_id, NULL, NULL, &cl_errno
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during creating context: %d\n", cl_errno);
		goto exit;
	}

	cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, NULL);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during creating command queue: %d\n", cl_errno);
		goto context_release;
	}
	
	PGMImage* input_image = read_pgm(INPUT_IMG_NAME);
	//mtype *source_matrix = matrix_create(MATRIX_WIDTH, MATRIX_HEIGHT);

	cl_mem input_buffer = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
		input_image->pitch * input_image->sizeY, 
		input_image->data, 
		&cl_errno
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during input buffer allocation\n");
		goto cl_input_buffer_release;
	}

	cl_mem output_buffer = clCreateBuffer(
		context, 
		CL_MEM_WRITE_ONLY, 
		input_image->pitch * input_image->sizeY, 
		NULL, 
		&cl_errno
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during output buffer allocation\n");
		goto cl_output_buffer_release;
	}
	cl_mem filter_buffer = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
		filter_size * filter_size * sizeof(ftype), 
		filter, 
		&cl_errno
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during filter buffer allocation\n");
		goto cl_filter_buffer_release;
	}
	
	cl_program program = get_cl_program(context, KERNEL_FILE_NAME);
	cl_errno = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during building program: %d\n", cl_errno);
		exit(0);
	}

	const size_t work_size[2] = {input_image->sizeX,	input_image->sizeY};
	const size_t group_size[2] = {WAVEFRONT_SIZE, 1};
	cl_kernel vertical_kernel = clCreateKernel(program, "image_vertical", NULL);
	clSetKernelArg(vertical_kernel, 0, sizeof(cl_mem), (void*)&input_buffer);
	clSetKernelArg(vertical_kernel, 1, sizeof(cl_mem), (void*)&output_buffer);
	clSetKernelArg(vertical_kernel, 2, sizeof(long int), (void*)&input_image->sizeX);
	clSetKernelArg(vertical_kernel, 4, sizeof(long int), (void*)&input_image->sizeY);
	clSetKernelArg(vertical_kernel, 5, sizeof(cl_mem), (void*)&filter_buffer);
	clSetKernelArg(vertical_kernel, 6, sizeof(int), (void*)&filter_size);
	clEnqueueNDRangeKernel(
		queue, 
		vertical_kernel, 
		2, 
		0, 
		work_size, 
		group_size, 
		0, 
		NULL, 
		NULL
	);
	clFinish(queue);
//	void* result_matrix[input_image->pitch * input_image->sizeY];
//	cl_errno = clEnqueueReadBuffer(
//		queue, 
//		output_buffer, 
//		CL_BLOCKING, 
//		0, 
//		MATRIX_SIZE_BYTES, 
//		result_matrix, 
//		0,
//		NULL, 
//		NULL
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
cl_program_release:
	clReleaseProgram(program);
cl_filter_buffer_release:
	clReleaseMemObject(filter_buffer);
cl_output_buffer_release:
	clReleaseMemObject(output_buffer);
cl_input_buffer_release:
	clReleaseMemObject(input_buffer);
queue_release:
	clReleaseCommandQueue(queue);
context_release:
	clReleaseContext(context);
exit:
	return 0;
}
