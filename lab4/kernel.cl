__kernel void kernel_fun(__global int* matrix, __global int* result_matrix, const int matrix_width, 
	const int max_matrix_width, const int max_matrix_height,
	const int width_in_blocks, const int height_in_blocks)
{
	int block_num = get_global_id(0);
	int block_x = block_num / width_in_blocks;
	int block_y = block_num - block_x * width_in_blocks;
	//printf("block_x: %d, block_y: %d, ", block_x, block_y);
	int local_x = get_global_id(1);
	int local_y = get_global_id(2);
	//printf("local_x: %d, local_y: %d, ", local_x, local_y);
	int global_x = local_x + block_x * width_in_blocks;
	int global_y = local_y + block_y * height_in_blocks;
	printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	if(global_x >= max_matrix_width || global_y >= max_matrix_height) {
		return;
	}
	int sum=0;
	for(int i =0; i <= global_x; i++) {
		for(int j = 0; j <= global_y; j++) {
			sum+=matrix[i * matrix_width + j];
		}
	}
	//printf("global_id(0): %d, global_id(1): %d, global_id(2): %d\n",get_global_id(0), get_global_id(1), get_global_id(2));
	result_matrix[global_x * matrix_width + global_y] = sum;
}