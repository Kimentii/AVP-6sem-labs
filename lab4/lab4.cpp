#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <intrin.h> 
#include "integral_image.h"
#pragma comment(lib, "OpenCL.lib")

const char KERNEL_FUN_NAME[] = "kernel_fun";
const char KERNEL_FILE_NAME[] = "E:/study/AVP/AVP-6sem-labs/lab4/kernel.cl";
const int NUM_OF_DIMENTIONS = 3;
const int MAX_SOURCE_SIZE = 10000;

cl_context context;
cl_device_id device_id;
cl_program program;
cl_command_queue command_queue;

void show_error(cl_int ret, const char* fun_name) {
	if (ret != 0) {
		printf("Error(%s): %d", fun_name, ret);
	}
}

void get_command_queue() {
	cl_int ret;
	cl_platform_id platform_id;
	cl_uint ret_num_platforms;
	/* получить доступные платформы */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	cl_uint ret_num_devices;
	/* получить доступные GPU */
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
	printf("Num of GPU: %d\n", ret_num_devices);
	if (ret != 0) {
		printf("clGetDeviceIDs: %d\n", ret);
	}

	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	if (ret != 0) {
		printf("clCreateContext: %d\n", ret);
	}

	/* создаем команду */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	if (ret != 0) {
		printf("clCreateCommandQueue: %d\n", ret);
	}
}

void show_device_info(cl_device_id dev_id) {
	cl_int ret;
	size_t max_work_group_size;
	ret = clGetDeviceInfo(dev_id, CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(max_work_group_size),
		&max_work_group_size, NULL);
	if (ret != 0) {
		printf("clGetDeviceInfo:%d\n", ret);
	}
	printf("Maximum number of work-items in a work-group: %d\n", max_work_group_size);

	cl_uint max_work_item_dimensions;
	ret = clGetDeviceInfo(dev_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dimensions),
		&max_work_item_dimensions, NULL);
	if (ret != 0) {
		printf("clGetDeviceInfo:%d\n", ret);
	}
	printf("Maximum dimensions that specify the global and local work-item IDs: %d\n", max_work_item_dimensions);

	cl_uint max_compute_units;
	ret = clGetDeviceInfo(dev_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units),
		&max_compute_units, NULL);
	if (ret != 0) {
		printf("clGetDeviceInfo:%d\n", ret);
	}
	printf("Num of compute units: %d\n", max_compute_units);

	size_t* max_work_item_sizes = (size_t*) malloc(sizeof(size_t) * max_work_item_dimensions);
	ret = clGetDeviceInfo(dev_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * max_work_item_dimensions,
		max_work_item_sizes, NULL);
	if (ret != 0) {
		printf("clGetDeviceInfo:%d\n", ret);
	}
	printf("Maximum number of work-items that can be specified in each dimension: ");
	for (int i = 0; i < max_work_item_dimensions; i++) {
		printf("%d, ", max_work_item_sizes[i]);
	}
	printf("\n");
	free(max_work_item_sizes);
}

void create_program() {
	cl_int ret;

	FILE *fp;
	size_t source_size;
	char *source_str;
	int i;

	try {
		fp = fopen(KERNEL_FILE_NAME, "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			_getch();
			exit(1);
		}
		source_str = (char *)malloc(MAX_SOURCE_SIZE);
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
		fclose(fp);
	}
	catch (int a) {
		printf("%f", a);
	}

	/* создать бинарник из кода программы */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	if (ret != 0) {
		printf("clCreateProgramWithSource: %d\n", ret);
	}

	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (ret != 0) {
		printf("clBuildProgram: %d\n", ret);
	}
}

void calculate_block(int* matrix, int* result_matrix, const int matrix_width, const int matrix_height,
	const int max_matrix_width, const int max_matrix_height,
	const int width_in_blocks, const int height_in_blocks) {

	const int block_width = matrix_width / width_in_blocks;
	const int block_height = matrix_height / height_in_blocks;
	const int num_of_blocks = width_in_blocks * height_in_blocks;

	int ret;
	cl_kernel kernel = clCreateKernel(program, KERNEL_FUN_NAME, &ret);
	if (ret != 0) {
		printf("clCreateKernel: %d\n", ret);
	}

	const int MATRIX_SIZE = matrix_width * matrix_height;
	cl_mem matrix_obj;
	cl_mem result_matrix_obj;

	matrix_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE * sizeof(cl_int), NULL, &ret);
	show_error(ret, "clCreateBuffer");
	result_matrix_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, MATRIX_SIZE * sizeof(cl_int), NULL, &ret);
	show_error(ret, "clCreateBuffer");

	ret = clEnqueueWriteBuffer(command_queue, matrix_obj, CL_TRUE, 0, MATRIX_SIZE * sizeof(cl_int), matrix, 0, NULL, NULL);
	show_error(ret, "clEnqueueWriteBuffer");

	ret = clSetKernelArg(kernel, 0, sizeof(matrix_obj), (void *)&matrix_obj);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 1, sizeof(result_matrix_obj), (void *)&result_matrix_obj);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 2, sizeof(int), &matrix_width);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 3, sizeof(int), &max_matrix_width);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 4, sizeof(int), &max_matrix_height);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 5, sizeof(int), &width_in_blocks);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 6, sizeof(int), &height_in_blocks);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 7, sizeof(int), &block_width);
	show_error(ret, "clSetKernelArg");
	ret = clSetKernelArg(kernel, 8, sizeof(int), &block_height);
	printf("Num of blocks: %d\n", num_of_blocks);
	size_t global_work_size[] = { num_of_blocks, block_width, block_height };
	unsigned __int64 start = __rdtsc();
	ret = clEnqueueNDRangeKernel(command_queue, kernel, NUM_OF_DIMENTIONS, NULL, global_work_size, NULL, 0, NULL, NULL);
	clFinish(command_queue);						// Ждем окончания работы ядер
	unsigned __int64 end = __rdtsc();
	if (ret != 0) {
		printf("clEnqueueNDRangeKernel: %d\n", ret);
	}
	printf("OpenCL time: %lld\n", end - start);
	ret = clEnqueueReadBuffer(command_queue, result_matrix_obj, CL_TRUE, 0, MATRIX_SIZE * sizeof(cl_int), result_matrix, 0, NULL, NULL);
	show_error(ret, "clEnqueueReadBuffer");
	/*printf("result:\n");
	for (int i = 0; i < matrix_height; i++) {
		for (int j = 0; j < matrix_height; j++) {
			printf("%5d", result_matrix[i*matrix_height + j]);
		}
		printf("\n");
	}*/
	//printf("Result time: %ld", end - start);
}

const int BLOCK_WIDTH = 128;
const int BLOCK_HEIGHT = 128;
int main()
{
	get_command_queue();
	create_program();
	show_device_info(device_id);

	int matrix_width=512, matrix_height=512;
	printf("input width: ");
	scanf("%d", &matrix_width);
	printf("input height: ");
	scanf("%d", &matrix_height);
	int width_in_blocks = matrix_width / BLOCK_WIDTH;
	if (!(width_in_blocks * BLOCK_WIDTH == matrix_width)) {
		width_in_blocks++;
	}
	int height_in_blocks = matrix_height / BLOCK_HEIGHT;
	if (!(height_in_blocks * BLOCK_HEIGHT == matrix_height)) {
		height_in_blocks++;
	}
	int required_matrix_width = width_in_blocks * BLOCK_WIDTH;
	int required_matrix_heigth = height_in_blocks * BLOCK_HEIGHT;
	printf("required_matrix_width: %d, required_matrix_heigth: %d\n", required_matrix_width, required_matrix_heigth);
	int* matrix = (int*)malloc(sizeof(int) * required_matrix_width * required_matrix_heigth);
	int* result_matrix_opencl = (int*)calloc(sizeof(int), required_matrix_width * required_matrix_heigth);
	int* result_matrix_c = (int*)calloc(sizeof(int), required_matrix_width * required_matrix_heigth);
	for (int i = 0; i < required_matrix_width; i++) {
		for (int j = 0; j < required_matrix_heigth; j++) {
			matrix[i * required_matrix_width + j] = 1;
			//printf("%5d", matrix[i * required_matrix_width + j]);
		}
		//printf("\n");
	}

	calculate_block(matrix, result_matrix_opencl, required_matrix_width,required_matrix_heigth,
		matrix_width,matrix_height, 
		width_in_blocks, height_in_blocks);
	//print_matrix(result_matrix_opencl, required_matrix_width, required_matrix_heigth);
	unsigned __int64 start = __rdtsc();
	integral_image_with_secret(matrix, result_matrix_c, required_matrix_width, required_matrix_heigth);
	unsigned __int64 end = __rdtsc();
	printf("With C: %lld\n", end - start);
	matrix_compare(result_matrix_opencl, result_matrix_c, (required_matrix_width * required_matrix_heigth));
	//print_matrix(result_matrix_c, required_matrix_width, required_matrix_heigth);
	//print_matrix(result_matrix_opencl, required_matrix_width, required_matrix_heigth);
	clReleaseDevice(device_id);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);
	clReleaseProgram(program);
	_getch();
    return 0;
}