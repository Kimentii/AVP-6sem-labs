// define FILTER_SIZE
#define GROUP_SIZE    64
//#define BANK_ROW_SIZE 256
//#define ROW_SIZE 8
//#define KERNEL_ROW_FETCH_NUM ROW_SIZE/GROUP_SIZE
#define LOCAL_W GROUP_SIZE
#define LOCAL_H GROUP_SIZE
#define LOCAL_RESULT_W (LOCAL_W - (FILTER_SIZE-1))
#define LOCAL_RESULT_H (LOCAL_H - (FILTER_SIZE-1))
#define BORDER_WIDTH (FILTER_SIZE/2)

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
		if ((x+i*GROUP_SIZE) < FILTER_SIZE*FILTER_SIZE) {
			//filter[x + i*GROUP_SIZE] = 1.0/(FILTER_SIZE*FILTER_SIZE);//global_filter[x + i*GROUP_SIZE];
			filter[x + i*GROUP_SIZE] = global_filter[x + i*GROUP_SIZE];
		}
	}

	__local uchar local_work[LOCAL_W * LOCAL_H];
	long local_work_offset = (LOCAL_RESULT_H)*gy*p + (LOCAL_RESULT_W)*gx;
	for (int i = 0; i < LOCAL_H; i++) {
		//uchar4 item_data = vload4(i*p + y*4, input_image);
		//vstore4(item_data, i*w + y*4, local_work);
		if (((gx * LOCAL_RESULT_W + x) < (w + BORDER_WIDTH*2)) && ((gy * LOCAL_RESULT_H + i) < (h + BORDER_WIDTH*2))) {
			local_work[i*LOCAL_W + x] = input_image[local_work_offset + i*p + x];
		}
	}
	__local uchar local_result[LOCAL_RESULT_H * LOCAL_RESULT_W];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// filtering
	for (int i = 0; i < LOCAL_RESULT_H; i++) {
		if ((x < LOCAL_RESULT_W) && ((gx * LOCAL_RESULT_W + x) < w) && ((gy * LOCAL_RESULT_H + i) < h)) {
			__private float result = 0;
			for (int n = 0; n < FILTER_SIZE; n++) {
				for (int m = 0; m < FILTER_SIZE; m++) {
					__private float filter_value = filter[n*FILTER_SIZE + m];
					__private uchar cell_value = local_work[(i+n)*LOCAL_W + x + m];
					//if (i == 0 && x == 0) 

					result += (filter_value * cell_value);
				}
			}
			local_result[i*LOCAL_RESULT_W + x] = (uchar)result;
		}
	}

	for (int i = 0; i < LOCAL_RESULT_H; i++) {
		if ((x < LOCAL_RESULT_W) && ((gx * LOCAL_RESULT_W + x) < w) && ((gy * LOCAL_RESULT_H + i) < h)) {
			output_image[(i + LOCAL_RESULT_H*gy)*w + LOCAL_RESULT_W*gx + x] =local_result[i*LOCAL_RESULT_W + x];
		}
	}
}
