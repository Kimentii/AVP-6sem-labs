#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <intrin.h> 
#pragma comment(lib, "OpenCL.lib")

const char KERNEL_FUN[] = "kernel_fun";
const char KERNEL_FUN_WITH_LOCAL[] = "kernel_fun2";
const char KERNEL_FUN_WITH_TRANSACTIONS[] = "kernel_fun3";
const char KERNEL_FILE_NAME[] = "E:/study/AVP/AVP-6sem-labs/lab5/kernel.cl";
const int NUM_OF_DIMENTIONS = 3;
const int MAX_SOURCE_SIZE = 10000;
const int BLOCK_WIDTH = 4;
const int BLOCK_HEIGHT = 4;
const int ELEMENTS_IN_TRANSACTION = 4;
typedef int TYPE;

void show_error(cl_int ret, const char* fun_name) {
	if (ret != 0) {
		printf("Error(%s): %d", fun_name, ret);
	}
}

cl_device_id get_device_id() {
	cl_int ret;
	cl_platform_id platform_id[2];
	cl_device_id device_id;
	cl_uint ret_num_platforms;
	/* получить доступные платформы */
	ret = clGetPlatformIDs(2, platform_id, &ret_num_platforms);
	//printf("ret_num_platforms: %ld\n", ret_num_platforms);

	cl_uint ret_num_devices;
	/* получить доступные GPU */
	ret = clGetDeviceIDs(platform_id[1], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
	//printf("Num of GPU: %d\n", ret_num_devices);
	if (ret != 0) {
		printf("clGetDeviceIDs: %d\n", ret);
	}
	return device_id;
}

cl_context get_context(cl_device_id device_id) {
	cl_int ret;
	cl_context context;

	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	if (ret != 0) {
		printf("clCreateContext: %d\n", ret);
	}
	return context;
}

cl_command_queue get_command_queue(cl_context context, cl_device_id device_id) {
	cl_int ret;
	cl_command_queue command_queue;
	/* создаем команду */
	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
	if (ret != 0) {
		printf("clCreateCommandQueue: %d\n", ret);
	}
	return command_queue;
}

cl_program create_program(cl_context context, cl_device_id device_id) {
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

	source_str[0] = '/';
	source_str[1] = '/';
	/* создать бинарник из кода программы */
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	if (ret != 0) {
		printf("clCreateProgramWithSource: %d\n", ret);
	}


	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (ret != CL_SUCCESS) {
		printf("clBuildProgram: %d\n", ret);
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *)malloc(log_size);
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("%s\n", log);
	}
	return program;
}

cl_kernel create_kernel(const char* str, cl_program program) {
	int ret;
	cl_kernel kernel = clCreateKernel(program, str, &ret);
	if (ret != 0) {
		printf("clCreateKernel: %d\n", ret);
	}
	return kernel;
}

int main()
{
    return 0;
}

