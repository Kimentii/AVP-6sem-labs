// define FILTER_SIZE
#define GROUP_SIZE    16
#define BANK_ROW_SIZE 256
#define ROW_SIZE 8
#define KERNEL_ROW_FETCH_NUM ROW_SIZE/GROUP_SIZE
#define LOCAL_W GROUP_SIZE
#define LOCAL_H GROUP_SIZE
#define LOCAL_RESULT_W (LOCAL_W - FILTER_SIZE)
#define LOCAL_RESULT_H (LOCAL_H - FILTER_SIZE)

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
	__local float filter[FILTER_SIZE * FILTER_SIZE];
	int filter_copy_times = FILTER_SIZE*FILTER_SIZE/GROUP_SIZE + 1;
	for (int i = 0; i < filter_copy_times; i++) {
		if ((x+i*GROUP_SIZE) < FILTER_SIZE*FILTER_SIZE) {
			filter[x + i*GROUP_SIZE] = global_filter[x + i*GROUP_SIZE];
		}
	}

	__local uchar local_work[LOCAL_W * LOCAL_H];
	long local_work_offset = (LOCAL_H - FILTER_SIZE/2)*gy*p + (LOCAL_W - FILTER_SIZE/2)*gx;
	for (int i = 0; i < LOCAL_H; i++) {
		//uchar4 item_data = vload4(i*p + y*4, input_image);
		//vstore4(item_data, i*w + y*4, local_work);
		local_work[i*LOCAL_W + x] = input_image[local_work_offset + i*p + x];
	}
	__local uchar local_result[LOCAL_RESULT_H * LOCAL_RESULT_W];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	// filtering
	if (x < LOCAL_RESULT_W) {
		for (int i = 0; i < LOCAL_RESULT_H; i++) {
			__private uchar result = 0;
			for (int n = 0; n < FILTER_SIZE; n++) {
				for (int m = 0; m < FILTER_SIZE; m++) {
					__private float filter_value = filter[n*FILTER_SIZE + m];
					__private uchar cell_value = local_work[(i+n)*LOCAL_W + x + m];
					result += (uchar)(filter_value * cell_value);
				}
			}
			local_result[(i + FILTER_SIZE/2)*LOCAL_W + x + FILTER_SIZE/2] = 3;// result;
		}
	}
	//__private long output_offset = (LOCAL_H - FILTER_SIZE)*gy*w + (LOCAL_W - FILTER_SIZE)*gx + i*w + x;
	for (int i = 0; i < LOCAL_RESULT_H; i++) {
		if (x < LOCAL_RESULT_W) {
			output_image[(LOCAL_RESULT_H*gy + i)*w + LOCAL_RESULT_W*gx + x] = local_result[i*LOCAL_RESULT_W + x];
		}
	}
	//printf("%s", "done\n");
}
