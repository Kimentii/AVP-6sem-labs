#include <CL/cl.h>
#include <stdio.h>
#include "cl_api.h"
#include "pgm.h"
#include "matrix.h"
#include "filter.h"
#if defined(WIN32) || defined(WIN64)
#include <conio.h>
#pragma comment(lib, "OpenCL.lib")
#endif

#define WAVEFRONT_SIZE 64
#define GLOBAL_BANK_ROW_SIZE 256

#define KERNEL_FILE_NAME "kernel.cl"
#define KERNEL_FUN "kernel_fun"
#define INPUT_IMG_NAME "image.pgm"
#define OUTPUT_IMG_NAME "out_image.pgm"
const int BLOCK_WIDTH = 256;
const int BLOCK_HEIGHT = 1;

int main()
{
	int filter_size = 3;
	ftype* filter = get_filter(filter_size, filter_size, FILTER_TYPE_BLUR);
	normalize_filter(filter, filter_size, filter_size, 100);
	//matrix_show(filter, filter_size, filter_size);

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

	PGMImage* input_image = mock_pgm(16, 16);//read_pgm(INPUT_IMG_NAME);
	//mtype *source_matrix = matrix_create(MATRIX_WIDTH, MATRIX_HEIGHT);
	input_image->pitch = input_image->sizeX;

	cl_mem input_buffer = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
		input_image->pitch * input_image->sizeY, 
		input_image->data, 
		&cl_errno
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during input buffer allocation: %d\n", cl_errno);
		goto cl_input_buffer_release;
	}

	void* result = malloc(input_image->pitch * input_image->sizeY);
	memset(result, 0, input_image->pitch * input_image->sizeY);
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
	char filter_size_define[32];
	if (sprintf(filter_size_define, "-DFILTER_SIZE=%d", filter_size) < 0) {
		printf("Can not set group size define for OpenCL program, exit\n");
		goto cl_filter_buffer_release;
	} else {
		//printf("Adding define: %s\n", group_size_define);
	}
	cl_errno = clBuildProgram(program, 1, &device_id, filter_size_define, NULL, NULL);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during building program: %d\n", cl_errno);
		exit(0);
	}

	//const size_t work_size[2] = {input_image->sizeX,	input_image->sizeY};
	const size_t work_size[2] = {16, 1};
	const size_t group_size[2] = {16, 1};
	cl_kernel kernel = clCreateKernel(program, KERNEL_FUN, &cl_errno);
	if (cl_errno != CL_SUCCESS) {
		printf("cl_errno: %d\n", cl_errno);
	}
	cl_errno = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&input_buffer);
	if (cl_errno != CL_SUCCESS) {
		printf("arg0 cl_errno: %d\n", cl_errno);
	}
	cl_errno = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output_buffer);
	if (cl_errno != CL_SUCCESS) {
		printf("arg1 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 2, sizeof(size_t), (void*)&input_image->sizeX);
	if (cl_errno != CL_SUCCESS) {
		printf("arg2 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 3, sizeof(size_t), (void*)&input_image->sizeY);
	if (cl_errno != CL_SUCCESS) {
		printf("arg3 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 4, sizeof(size_t), (void*)&input_image->pitch);
	if (cl_errno != CL_SUCCESS) {
		printf("arg3 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&filter_buffer);
	clEnqueueNDRangeKernel(
		queue, 
		kernel, 
		2, 
		0, 
		work_size, 
		group_size, 
		0, 
		NULL, 
		NULL
	);
	clFinish(queue);
	cl_errno = clEnqueueReadBuffer(
		queue,
		output_buffer, 
		CL_BLOCKING, 
		0, 
		input_image->pitch * input_image->sizeY, 
		result, 
		0,
		NULL, 
		NULL
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during reading result matrix: %d\n", cl_errno);
	}
	clFinish(queue);

	for (int i = 0; i < input_image->sizeY; i++) {
		for (int j = 0; j < input_image->sizeX; j++) {
			printf("%d\t", ((unsigned char*)(result))[i*input_image->sizeX+j]);
		}
		printf("\n");
	}
//	printf("========");
//	for (int i = 0; i < input_image->sizeY; i++) {
//		for (int j = 0; j < input_image->sizeX; j++) {
//			printf("%d ", ((unsigned char*)(result))[i*input_image->sizeX+j]);
//		}
//		printf("\n");
//	}
//
//	
////
////	long long c_time_start = rdtsc();
////	mtype* c_result = matrix_int_img(source_matrix, MATRIX_HEIGHT, MATRIX_WIDTH);
////	long long c_time = rdtsc() - c_time_start;
////
////	if(memcmp(c_result, result_matrix, MATRIX_SIZE_BYTES) == 0) {
////		printf("Validation success!\n");
////	}	else {
////		printf("Validation failed!\n");
////	}
////		printf("CL time: \t%lld\n", cl_time);
////		printf("C time: \t%lld\n", c_time);
////	//matrix_show(result_matrix, MATRIX_HEIGHT, MATRIX_WIDTH);
////	//printf("=== C ===\n");
////	//matrix_show(c_result, MATRIX_HEIGHT, MATRIX_WIDTH);
////
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
#if defined(WIN32) || defined(WIN64)
_getch();
#endif
	return 0;
}
