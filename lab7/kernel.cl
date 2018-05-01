// define FILTER_SIZE
//#define BANK_ROW_SIZE 256
//#define ROW_SIZE 8
//#define KERNEL_ROW_FETCH_NUM ROW_SIZE/GROUP_SIZE
#define GROUP_SIZE_X	9
#define GROUP_SIZE_Y	4
#define GROUP_SIZE		(GROUP_SIZE_X * GROUP_SIZE_Y)
#define LOCAL_W GROUP_SIZE_X
#define LOCAL_H GROUP_SIZE_Y
#define ITEM_RESULT_W (LOCAL_W - 3*(FILTER_SIZE-1))
#define ITEM_RESULT_H (LOCAL_H - (FILTER_SIZE-1))
#define BORDER_WIDTH (FILTER_SIZE/2)
#define ROWS_PER_GROUP 1
#define ROWS_PER_ITEM 1

__kernel void kernel_fun(
	__global unsigned char* input_image, 
	__global unsigned char* output_image,
	long w,
	long h,
	long p,
	__global float* global_filter
	)
{
	int x = get_local_id(0);
	int y = get_local_id(1);
	int gx = get_group_id(0);
	int gy = get_group_id(1);
	// 1. load filter in local memory
	// 2. load group data in local memory
	// 3. apply filter
	__local float filter[FILTER_SIZE * FILTER_SIZE];
	int filter_copy_times = FILTER_SIZE*FILTER_SIZE/GROUP_SIZE + 1;
	for (int i = 0; i < filter_copy_times; i++) {
		if ((x + y*GROUP_SIZE_X + i*GROUP_SIZE) < FILTER_SIZE*FILTER_SIZE) {
			//filter[x + i*GROUP_SIZE] = 1.0/(FILTER_SIZE*FILTER_SIZE);//global_filter[x + i*GROUP_SIZE];
			filter[x + y*GROUP_SIZE_X + i*GROUP_SIZE] = global_filter[x + y*GROUP_SIZE_X + i*GROUP_SIZE];
		}
	}

	__local uchar local_work[LOCAL_W * LOCAL_H];
	long local_work_offset = ITEM_RESULT_H*gy*p + ITEM_RESULT_W*gx;
	for (int i = 0; i < ROWS_PER_ITEM; i++) {
		if ((gy*ITEM_RESULT_H + ROWS_PER_ITEM*y + i) < (h + BORDER_WIDTH*2)) {
			local_work[(y*ROWS_PER_ITEM + i)*LOCAL_W + x] = y;//input_image[local_work_offset + (y*ROWS_PER_ITEM + i)*p + x];
			//long4 item_data = vload4((gy*ROWS_PER_GROUP + ROWS_PER_ITEM*y + i)*p + gx*GROUP_SIZE_X, (__global long*)input_image);
			//vstore4(item_data, (y*ROWS_PER_ITEM + i)*w, (__local long*)local_work);
		}
	}
	__local uchar local_result[ITEM_RESULT_H * ITEM_RESULT_W];
//	barrier(CLK_LOCAL_MEM_FENCE);
//	
//	// filtering
//	for (int i = 0; i < ITEM_RESULT_H; i++) {
//		local_reslut[i] = 7;
////		if ((x < ITEM_RESULT_W) && ((gx * ITEM_RESULT_W + x) < w) && ((gy * ITEM_RESULT_H + i) < h)) {
////			__private float result = 0;
////			for (int n = 0; n < FILTER_SIZE; n++) {
////				for (int m = 0; m < FILTER_SIZE; m++) {
////					__private float filter_value = filter[n*FILTER_SIZE + m];
////					__private uchar cell_value = local_work[(i+n)*LOCAL_W + x + m];
////					//if (i == 0 && x == 0) 
////
////					result += (filter_value * cell_value);
////				}
////			}
////			local_result[i*ITEM_RESULT_W + x] = (uchar)result;
////		}
//	}
//
//	if (x < 3 && y < 2) {
//		output_image[y*w*3 + gx*3 + x] = 9;
//	}
	for (int i = 0; i < ROWS_PER_ITEM; i++) {
			if ((x < ITEM_RESULT_W) && ((gx * ITEM_RESULT_W + x) < w*3) && ((gy*ITEM_RESULT_H + y*ROWS_PER_ITEM + i) < h)) {
				output_image[(i + y*ROWS_PER_ITEM + ITEM_RESULT_H*gy)*3*w + ITEM_RESULT_W*gx + x] 
				 = local_work[(i + y*ROWS_PER_ITEM)*ITEM_RESULT_W + x];
			}
	}
}
