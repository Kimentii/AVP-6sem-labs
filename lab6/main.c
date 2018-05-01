#include <CL/cl.h>
#include <stdio.h>
#include "cl_api.h"
#include "pgm.h"
#include "matrix.h"
#include "filter.h"
#include "string.h"
#include "cpu_filter.h"
#include "time_measure.h"
#if defined(WIN32) || defined(WIN64)
#include <conio.h>
#pragma comment(lib, "OpenCL.lib")
#endif

#define WAVEFRONT_SIZE 64
#define GROUP_SIZE WAVEFRONT_SIZE
#define GLOBAL_BANK_ROW_SIZE 256

#define KERNEL_FILE_NAME "kernel.cl"
#define KERNEL_FUN "kernel_fun"
#define INPUT_IMG_NAME "image.pgm"
#define OUTPUT_IMG_NAME "out_image.pgm"

const int BLOCK_WIDTH = 256;
const int BLOCK_HEIGHT = 1;

int main(int argc, char* argv[])
{
	char* input_image_name;
	int filter_size = atoi(argv[1]);
	int computing_group_size = GROUP_SIZE - (filter_size - 1);
	if (argc > 2) {
		input_image_name = argv[2];
	} else {
		input_image_name = INPUT_IMG_NAME;
	}
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

	PGMImage* input_image = read_pgm(input_image_name);
	size_t imgX = input_image->sizeX;
	size_t imgY = input_image->sizeY;
	add_frame(input_image, filter_size/2);
	resize_image(input_image, BLOCK_WIDTH, 1);

	time_snap();
	cl_mem input_buffer = clCreateBuffer(
		context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
		input_image->pitch * (imgY + 2 * input_image->frame_size), 
		input_image->data, 
		&cl_errno
	);
	if (cl_errno != CL_SUCCESS) {
		printf("Error during input buffer allocation: %d\n", cl_errno);
		goto cl_input_buffer_release;
	}

	unsigned char* gpu_result = malloc(input_image->pitch * input_image->sizeY);
	memset(gpu_result, 0, input_image->pitch * input_image->sizeY);
	cl_mem output_buffer = clCreateBuffer(
		context, 
		CL_MEM_WRITE_ONLY, 
		imgX * imgY, 
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
	uint64_t buffers_allocation_time = time_elapsed();
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
	uint64_t build_time = time_elapsed();
	
	size_t groups_x = imgX/computing_group_size;
	if (imgX % computing_group_size != 0) groups_x++;

	size_t groups_y = imgY/computing_group_size;
	if (imgY % computing_group_size != 0) groups_y++;

	printf("work size: [%ld, %ld]\n", groups_x, groups_y); 
	const size_t work_size[2] = {groups_x * GROUP_SIZE, groups_y};
	const size_t group_size[2] = {GROUP_SIZE, 1};

	cl_kernel kernel = clCreateKernel(program, KERNEL_FUN, &cl_errno);
	if (cl_errno != CL_SUCCESS) {
		printf("create kernel cl_errno: %d\n", cl_errno);
	}
	cl_errno = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&input_buffer);
	if (cl_errno != CL_SUCCESS) {
		printf("arg0 cl_errno: %d\n", cl_errno);
	}
	cl_errno = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output_buffer);
	if (cl_errno != CL_SUCCESS) {
		printf("arg1 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 2, sizeof(size_t), (void*)&imgX);
	if (cl_errno != CL_SUCCESS) {
		printf("arg2 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 3, sizeof(size_t), (void*)&imgY);
	if (cl_errno != CL_SUCCESS) {
		printf("arg3 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 4, sizeof(size_t), (void*)&input_image->pitch);
	if (cl_errno != CL_SUCCESS) {
		printf("arg4 cl_errno: %d\n", cl_errno);
	}
	clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&filter_buffer);
	if (cl_errno != CL_SUCCESS) {
		printf("arg5 cl_errno: %d\n", cl_errno);
	}
	uint64_t args_prep_time = time_elapsed();
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
	uint64_t gpu_exec_time = time_elapsed();
	cl_errno = clEnqueueReadBuffer(
		queue,
		output_buffer, 
		CL_BLOCKING, 
		0, 
		imgX * imgY, 
		gpu_result, 
		0,
		NULL, 
		NULL
	);
	uint64_t gpu_all_time = time_elapsed();
	if (cl_errno != CL_SUCCESS) {
		printf("Error during reading result matrix: %d\n", cl_errno);
	}
	time_snap();
	unsigned char* cpu_result = filter_via_cpu(input_image, filter, filter_size);
	uint64_t cpu_time = time_elapsed();
	printf("input_image size: [%ld, %ld]\n", imgX, imgY);
	printf("Memcmp: %d\n", memcmp(cpu_result, gpu_result, imgX*imgY));
	printf("%-25s: %15lu\n", "GPU time", gpu_all_time);
	printf("%-25s: %15lu\n", "--> buf alloc time", buffers_allocation_time);
	printf("%-25s: %15lu\n", "--> build time", build_time - buffers_allocation_time);
	printf("%-25s: %15lu\n", "--> args prep time", args_prep_time - build_time);
	printf("%-25s: %15lu\n", "--> result read time", gpu_all_time - gpu_exec_time);
	printf("%-25s: %15lu\n", "--> execution time", gpu_exec_time - args_prep_time);
	printf("%-25s: %15lu\n", "CPU time", cpu_time);

	PGMImage gpu_image, cpu_image;
	gpu_image.sizeX = imgX;
	gpu_image.sizeY = imgY;
	gpu_image.data = gpu_result;
	gpu_image.pitch = imgX;
	gpu_image.frame_size = 0;
	cpu_image.sizeX = imgX;
	cpu_image.sizeY = imgY;
	cpu_image.pitch = imgX;
	cpu_image.frame_size = 0;
	cpu_image.data = cpu_result;

	write_pgm(&gpu_image, "gpu_result.pgm");
	write_pgm(&cpu_image, "cpu_result.pgm");

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
