#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#pragma comment(lib, "OpenCL.lib")

const char KERNEL_NAME[] = "kernel.cl";
const int MAX_SOURCE_SIZE = 100000;

cl_context context;
cl_device_id device_id;

cl_command_queue get_command_queue() {
	cl_int ret;
	cl_platform_id platform_id;
	cl_uint ret_num_platforms;
	/* получить доступные платформы */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	cl_uint ret_num_devices;
	/* получить доступные GPU */
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	cl_command_queue command_queue;
	/* создаем команду */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	return command_queue;
}

cl_program create_program() {
	cl_int ret;
	cl_program program = NULL;

	FILE *fp;
	size_t source_size;
	char *source_str;
	int i;

	try {
		fp = fopen(KERNEL_NAME, "r");
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

	/* скомпилировать программу */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	return program;
}

int main()
{
	cl_command_queue command_queue = get_command_queue();
	cl_program program = create_program();
    return 0;
}