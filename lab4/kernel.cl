__kernel void kernel(__global int* matrix,__global bool* status_map, int x, int y)
{
	const int MATRIX_LENGTH = 10;
	const int MATRIX_SIZE = MATRIX_LENGTH * MATRIX_LENGTH;
	// получаем текущий id.
	int gid = get_global_id(0);
	printf("Started kernel: %d", gid);
	if (x == y) {
		if(x == 0 && y == 0) {
			status_map[x * MATRIX_LENGTH + y] = true;
			printf("set true to x:%d, y:%d\n", x, y);
			return;
		} else {
			while (true) {
				if(status_map[(x-1)*MATRIX_LENGTH + y] != true)
					continue;
				if(status_map[x*MATRIX_LENGTH + y -1] != true)
					continue;
				if(status_map[(x-1)*MATRIX_LENGTH + y -1] != true)
					continue;
				break;
			}
			matrix[x*MATRIX_LENGTH + y] = matrix[x*MATRIX_LENGTH + y] - matrix[(x-1)*MATRIX_LENGTH + y - 1] +
				matrix[(x-1)*MATRIX_LENGTH + y] + matrix[x*MATRIX_LENGTH + y -1];
			status_map[x * MATRIX_LENGTH + y] = true;
			printf("set true to x:%d, y:%d\n", x, y);
			}
		status_map[x * MATRIX_LENGTH + y] = true;
	// diagonal
	} else if (x > y) {
		if(y-1 < 0) {
			while (true) {
				if(status_map[(x-1)*MATRIX_LENGTH + y] != true)
					continue;
				break;
			}
			matrix[x * MATRIX_LENGTH + y] = matrix[x * MATRIX_LENGTH + y] + matrix[(x-1)*MATRIX_LENGTH + y];
			printf("set to x:%d y:%d this: %d\n", x, y, matrix[x * MATRIX_LENGTH + y]);
		} else {
			while (true) {
				if(status_map[(x-1)*MATRIX_LENGTH + y] != true)
					continue;
				if(status_map[x*MATRIX_LENGTH + y -1] != true)
					continue;
				if(status_map[(x-1)*MATRIX_LENGTH + y -1] != true)
					continue;
				break;
			}
			matrix[x*MATRIX_LENGTH + y] = matrix[x*MATRIX_LENGTH + y] - matrix[(x-1)*MATRIX_LENGTH + y - 1] +
						matrix[(x-1)*MATRIX_LENGTH + y] + matrix[x*MATRIX_LENGTH + y -1];
			printf("set to x:%d y:%d this: %d\n", x, y, matrix[x * MATRIX_LENGTH + y]);
		}
		status_map[x * MATRIX_LENGTH + y] = true;
		printf("set true to x:%d, y:%d\n", x, y);
	// vertical
	} else {
		if(x-1 < 0) {
			while (true) {
				if(status_map[x*MATRIX_LENGTH + y -1] != true)
					continue;
				break;
			}
			matrix[x*MATRIX_LENGTH + y] = matrix[x*MATRIX_LENGTH + y] + matrix[x*MATRIX_LENGTH + y -1];
			printf("set to x:%d y:%d this: %d\n", x, y, matrix[x * MATRIX_LENGTH + y]);
		} else {
			while (true) {
				if(status_map[(x-1)*MATRIX_LENGTH + y] != true)
					continue;
				if(status_map[x*MATRIX_LENGTH + y -1] != true)
					continue;
				if(status_map[(x-1)*MATRIX_LENGTH + y -1] != true)
					continue;
				break;
			}
			matrix[x*MATRIX_LENGTH + y] = matrix[x*MATRIX_LENGTH + y] - matrix[(x-1)*MATRIX_LENGTH + y - 1] +
						matrix[(x-1)*MATRIX_LENGTH + y] + matrix[x*MATRIX_LENGTH + y -1];
			printf("set to x:%d y:%d this: %d\n", x, y, matrix[x * MATRIX_LENGTH + y]);
		}
		status_map[x * MATRIX_LENGTH + y] = true;
		printf("set true to x:%d, y:%d\n", x, y);
	// horizontal
	}
	//printf("gid: %d x:%d y:%d\n", gid, x ,y);
	/*for(int i =0;i<MATRIX_LENGTH;i++){
		for(int j = 0; j < MATRIX_LENGTH;j++){
			printf("%5d", matrix[i*MATRIX_LENGTH + j]);
		}
		printf("\n");
	}*/
}