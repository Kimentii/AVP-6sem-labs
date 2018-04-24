// define FILTER_SIZE
#define GROUP_SIZE    2
#define BANK_ROW_SIZE 256
#define ROW_SIZE 8
#define KERNEL_ROW_FETCH_NUM ROW_SIZE/GROUP_SIZE
#define LOCAL_W (GROUP_SIZE + ((FILTER_SIZE/2)*2))
#define LOCAL_H (ROW_SIZE + ((FILTER_SIZE/2)*2))

__kernel void kernel_fun(
	__global unsigned char* input_image, 
	__global unsigned char* output_image,
	long h,
	long w,
	long p,
	__global float* global_filter
	)
{
	int x = get_local_id(0);
	int y = get_local_id(1);
	int gx = get_group_id(0);
	int gy = get_group_id(1);
	//input_image[3] = 9;//input_image[y*w+x];
	//output_image[3] = 9;//input_image[y*w+x];
	// 1. load filter in local memory
	// 2. load group data in local memory
	// 3. apply filter
	//int a = 5;
	__local float filter[FILTER_SIZE * FILTER_SIZE];
	__local uchar local_work[LOCAL_W * LOCAL_H];
	if (y < FILTER_SIZE*FILTER_SIZE) {
		filter[x] = global_filter[y];
	}
	for (int i = 0; i < LOCAL_H; i++) {
		uchar4 item_data = vload4(i*p + y*4, input_image);
		vstore4(item_data, i*w + y*4, local_work);
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	// filtering
//	for (int i = 0; i < ROW_SIZE; i++) {
//		__private cell_value = 0;
//		for (int n = 0; n < FILTER_SIZE; n++) {
//			for (int m = 0; m < FILTER_SIZE; m++) {
//				__private float filter_value = filter[n*FILTER_SIZE + m];
//				__private char neighbour = 0;
//				__private long fetch_addr = (y - FILTER_SIZE/2 + n)*p + (i - FILTER_SIZE/2 + m);   
//				if (fetch_addr < 0) {
//					neighbour = 0;
//				} else {
//					neighbour = input_image[fetch_addr];
//				}
//				cell_value += (uchar)(filter_value * neighbour);
//			}
//		}
//		output_image[y*p + i] = cell_value;
//	}
//
//	for (int i = 0; i < KERNEL_ROW_FETCH_NUM; i++) {
//		for (int j = 0; j < 4; j++) {
//			output_image[y*w + i*4 + j] = local_work[y*w + i*4 + j];//(unsigned char)(filter[filter_value]);
//		}
//	}
	//printf("%s", "done\n");
}
