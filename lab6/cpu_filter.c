#include "cpu_filter.h"

PGMImage* filter_via_cpu(const PGMImage* image, ftype* filter, const int filter_size)
{
	PGMImage* output_image = malloc(sizeof(PGMImage));
	*output_image = *image;
	output_image->data = malloc(sizeof(unsigned char) * output_image->pitch
		* output_image->sizeY);

	for (int i = 0; i < image->sizeY; i++) {
		for (int j = 0; j < image->pitch; j++) {
			int mtrx_i = -filter_size / 2;
			float res = 0;
			for (int fil_i = 0; fil_i < filter_size; fil_i++, mtrx_i++) {
				int mtrx_j = -filter_size / 2;
				for (int fil_j = 0; fil_j < filter_size; fil_j++, mtrx_j++) {
					if ((i + mtrx_i < 0) || (j + mtrx_j < 0)
						|| (i + mtrx_i >= image->sizeY) || (j + mtrx_j >= image->pitch)) {
						continue;
					}
					else {
						res += (filter[fil_i*filter_size + fil_j]
							* image->data[(i + mtrx_i) * image->pitch + j + mtrx_j]);
					}
				}
			}
			output_image->data[i * image->pitch + j] = res;
		}
	}
	return output_image;
}
