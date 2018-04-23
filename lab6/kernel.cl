//
__kernel void kernel_fun(__global char* input_image, __global char* output_image,
	const int image_width)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	output_image[y * image_width + x] = input_image[y * image_width + x] - 100;

	//printf("%s", "done\n");
}