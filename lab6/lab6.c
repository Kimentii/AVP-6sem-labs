#include <CL/cl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32) || defined(WIN64)
#include <conio.h>
#endif
#include "cl_wrapper.h"
#include "loadpgm.h"
#pragma comment(lib, "OpenCL.lib")

const char KERNEL_FUN[] = "kernel_fun";
const char KERNEL_FILE_NAME[] = "kernel.cl";
const int NUM_OF_DIMENTIONS = 3;

int main()
{
	cl_device_id device_id = get_device_id();
	cl_context context = get_context(device_id);
	cl_command_queue command_queue = get_command_queue(context, device_id);
	cl_program program = create_program(context, device_id, KERNEL_FILE_NAME);
	cl_kernel kernel = create_kernel(KERNEL_FUN, program);
	
	cl_image_desc desc;
	cl_mem input_image;

	
	cl_image_format format;
	format.image_channel_data_type = CL_UNORM_INT8;
	format.image_channel_order = CL_INTENSITY;

	cl_int ret;
	PGMImage* image = read_pgm("image.pgm");
	write_pgm(image, "image2.pgm");
	
	memset(&desc, 0, sizeof(desc));
	desc.image_type = CL_MEM_OBJECT_IMAGE2D;
	desc.image_width = image->sizeX;
	desc.image_height = image->sizeY;

	/*input_image = clCreateImage(
		context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		&format,
		&desc,
		&image->data,
		&ret);
	show_error(ret, "clCreateImage");*/

	size_t global_work_size[] = { 5, 5, 5 };
	ret = clEnqueueNDRangeKernel(command_queue, kernel, NUM_OF_DIMENTIONS, NULL, global_work_size, NULL, 0, NULL, NULL);
	clFinish(command_queue);

	#if defined(WIN32) || defined(WIN64)
	_getch();
	#endif
	return 0;
}

