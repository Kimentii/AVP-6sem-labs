__kernel void kernel_fun(__global int* matrix, __global int* result_matrix, __local int* local_matrix,
	const int matrix_width, const int read_matrix_width, const int real_matrix_height,
	const int width_in_blocks, const int height_in_blocks,
	const int block_width, const int block_height)
{
	int block_num = get_global_id(0);
	int block_x = block_num / width_in_blocks;
	int block_y = block_num - block_x * width_in_blocks;
	//printf("block_x: %d, block_y\n: %d, ", block_x, block_y);
	int local_x = get_global_id(1);
	int local_y = get_global_id(2);
	//printf("local_x: %d, local_y: %d\n", local_x, local_y);
	int global_x = local_x + block_x * block_width;
	int global_y = local_y + block_y * block_height;
	//printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	if(global_x >= read_matrix_width || global_y >= real_matrix_height) {
		//printf("!!!!!!!global_x: %d, global_y: %d\n", global_x, global_y);
		return;
	}
	local_matrix[global_x * matrix_width + global_y] = matrix[global_x * matrix_width + global_y];
	barrier(CLK_GLOBAL_MEM_FENCE);
	barrier(CLK_LOCAL_MEM_FENCE);
	/*if(local_x == 9 && local_y == 9){
		for(int i =0; i < block_width; i++){
			for(int j = 0; j < block_height; j++) {
				printf("%5d", local_matrix[i * block_width + j]);
			}
			printf("\n");
		}
	}*/
	//printf("global_x: %d, global_y: %d\n", global_x, global_y);
	//printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	int sum=0;
	for(int i =0; i <= global_x; i++) {
		for(int j = 0; j <= global_y; j++) {
			//sum += matrix[i * matrix_width + j];
			sum += local_matrix[i * matrix_width + j];
		}
	}
	//printf("global_id(0): %d, global_id(1): %d, global_id(2): %d\n",get_global_id(0), get_global_id(1), get_global_id(2));
	result_matrix[global_x * matrix_width + global_y] = sum;
}