#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#pragma comment(lib, "OpenCL.lib")

const char KERNEL_FUN_NAME[] = "kernel_fun";
const char KERNEL_FILE_NAME[] = "kernel.cl";
const int MAX_SOURCE_SIZE = 10000;

cl_context context;
cl_device_id device_id;
cl_program program;

void show_error(cl_int ret, const char* fun_name) {

}

cl_command_queue get_command_queue() {
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

	cl_command_queue command_queue;
	/* создаем команду */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	if (ret != 0) {
		printf("clCreateCommandQueue: %d\n", ret);
	}
	return command_queue;
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

int main()
{
	cl_command_queue command_queue = get_command_queue();
	create_program();
	show_device_info(device_id);

	int ret;
	cl_kernel kernel = clCreateKernel(program, KERNEL_FUN_NAME, &ret);
	if (ret != 0) {
		printf("clCreateKernel: %d\n", ret);
	}
	size_t global_work_size[1] = { 10 };
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	clFinish(command_queue);						// Ждем окончания работы ядер
	if (ret != 0) {
		printf("clEnqueueNDRangeKernel: %d\n", ret);
	}
	_getch();
    return 0;
}