__kernel void kernel_fun(__global int* matrix, __global int* result_matrix)
{
	const int MATRIX_LENGTH = 4;
	int x = get_global_id(0);
	int y = get_global_id(1);
	int sum=0;
	for(int i =0; i <= x; i++) {
		for(int j = 0; j <= y; j++) {
			sum+=matrix[i * MATRIX_LENGTH + j];
		}
	}
	printf("global_id(0): %d, global_id(1): %d\n",get_global_id(0), get_global_id(1));
	result_matrix[x * MATRIX_LENGTH + y] = sum;
}