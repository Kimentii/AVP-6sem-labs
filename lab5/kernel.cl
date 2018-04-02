// this string is kostil(trust me, it is nessery)
__kernel void kernel_fun(__global int* matrix, __global int* result_matrix,
	const int matrix_width, const int real_matrix_width, const int real_matrix_height,
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
	if(global_x >= real_matrix_width || global_y >= real_matrix_height) {
		//printf("!!!!!!!global_x: %d, global_y: %d\n", global_x, global_y);
		return;
	}
	//printf("global_x: %d, global_y: %d\n", global_x, global_y);
	//printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	int sum=0;
	for(int i =0; i <= global_x; i++) {
		for(int j = 0; j <= global_y; j++) {
			sum += matrix[i * matrix_width + j];
		}
	}
	//printf("global_id(0): %d, global_id(1): %d, global_id(2): %d\n",get_global_id(0), get_global_id(1), get_global_id(2));
	result_matrix[global_x * matrix_width + global_y] = sum;
}

__kernel void kernel_fun2(__global int* matrix, __global int* result_matrix, __local int* local_matrix,
	const int matrix_width, const int real_matrix_width, const int real_matrix_height,
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

	//printf("groud_id(0): %d, groud_id(1): %d, groud_id(2): %d\n", get_group_id(0), get_group_id(1), get_group_id(2));

	//printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	if(global_x >= real_matrix_width || global_y >= real_matrix_height) {
		//printf("!!!!!!!global_x: %d, global_y: %d\n", global_x, global_y);
		return;
	}
	local_matrix[global_x * matrix_width + global_y] = matrix[global_x * matrix_width + global_y];
	barrier(CLK_GLOBAL_MEM_FENCE);
	barrier(CLK_LOCAL_MEM_FENCE);
	/*if(global_x == 39 && global_y == 39) {
		for(int i =0; i < real_matrix_width; i++){
			for(int j = 0; j < real_matrix_height; j++) {
				printf("%5d", local_matrix[i * real_matrix_width + j]);
			}
			printf("\n");
		}
	}*/
	//atomic_inc(result_matrix);

	
	//printf("global_x: %d, global_y: %d\n", global_x, global_y);
	//printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	int sum=0;
	for(int i = 0; i <= global_x; i++) {
		for(int j = 0; j <= global_y; j++) {
			//sum += matrix[i * matrix_width + j];
			sum += local_matrix[i * matrix_width + j];
		}
	}
	//printf("global_id(0): %d, global_id(1): %d, global_id(2): %d\n",get_global_id(0), get_global_id(1), get_global_id(2));
	//atomic_add(result_matrix + global_x * matrix_width + global_y, sum);
	result_matrix[global_x * matrix_width + global_y] = sum;
	//printf("Ended: global_x: %d, global_y: %d\n", global_x, global_y);
	
}
/*
__kernel void kernel_fun3_x(__global int* matrix, __global int* result_matrix, __local int* local_matrix,
	const int matrix_width, const int real_matrix_width, const int real_matrix_height,
	const int width_in_blocks, const int height_in_blocks,
	const int block_width, const int block_height)
{
	const int ELEMENTS_IN_TRANSACTION = 4;
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
	if(global_x >= real_matrix_width || global_y >= real_matrix_height) {
		//printf("!!!!!!!global_x: %d, global_y: %d\n", global_x, global_y);
		return;
	}
	for(int i =0; i<ELEMENTS_IN_TRANSACTION;i++) {
		local_matrix[(lobal_x * ELEMENTS_IN_TRANSACTION + i) * matrix_width + global_y] = 
			matrix[(global_x * ELEMENTS_IN_TRANSACTION + i) * matrix_width + global_y];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	//printf("global_x: %d, global_y: %d\n", global_x, global_y);
	//printf("block_x: %d, block_y: %d, global_x: %d, global_y: %d, local_x: %d, local_y: %d\n", block_x, block_y, global_x, global_y, local_x, local_y);
	int sum=0;
	for(int i = 0; i <= global_x; i++) {
		for(int j = 0; j <= global_y; j++) {
			//sum += matrix[i * matrix_width + j];
			sum += local_matrix[i * matrix_width + j];
		}
	}
	//printf("global_id(0): %d, global_id(1): %d, global_id(2): %d\n",get_global_id(0), get_global_id(1), get_global_id(2));
	result_matrix[global_x * matrix_width + global_y] = sum;
}*/