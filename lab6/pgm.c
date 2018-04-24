#include "pgm.h"

PGMImage* read_pgm(const char *filename)
{
	char buff[3];
	PGMImage *result;
	FILE *fp;
	int maxval;

	fp = fopen(filename, "rb");
	if (!fp)
	{
		fprintf(stderr, "Unable to open file `%s'\n", filename);
#if defined(WIN32) || defined(WIN64)
		_getch();
#endif
		exit(1);
	}

	if (!fgets(buff, sizeof(buff), fp))
	{
		perror(filename);
#if defined(WIN32) || defined(WIN64)
		_getch();
#endif
		exit(1);
	}

	if (buff[0] != 'P' || buff[1] != '5')
	{
		fprintf(stderr, "Invalid image format (must be `P5')\n");
#if defined(WIN32) || defined(WIN64)
		_getch();
#endif
		exit(1);
	}

	result = (PGMImage *)malloc(sizeof(PGMImage));
	if (!result)
	{
		fprintf(stderr, "Unable to allocate memory\n");
#if defined(WIN32) || defined(WIN64)
		_getch();
#endif
		exit(1);
	}

	fgetc(fp);
	while (fgetc(fp) != '\n');

	if (fscanf(fp, "%lu %lu", &result->sizeX, &result->sizeY) != 2)
	{
		fprintf(stderr, "Error1 loading image `%s'\n", filename);
#if defined(WIN32) || defined(WIN64)
		_getch();
#endif
		exit(1);
	}


	if (fscanf(fp, "%d", &maxval) != 1)
	{
		fprintf(stderr, "Error loading image `%s'\n", filename);
		exit(1);
	}

	while (fgetc(fp) != '\n');

	result->data = (unsigned char *)malloc(result->sizeX * result->sizeY);
	if (!result)
	{
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	if (fread(result->data, result->sizeX, result->sizeY, fp) != result->sizeY)
	{
		fprintf(stderr, "Error loading image `%s'\n", filename);
		exit(1);
	}
	fclose(fp);

	result->pitch = 0;
	result->frame_size = 0;
	return result;
}

void resize_image(PGMImage* image, const int block_width, const int block_height)
{
	int image_width = image->sizeX + 2 * image->frame_size;
	int image_heigth = image->sizeY + 2 * image->frame_size;
	int width_in_blocks = image_width / block_width;
	if (!(width_in_blocks * block_width == image_width)) {
		width_in_blocks++;
	}
	int height_in_blocks = image_heigth / block_height;
	if (!(height_in_blocks * block_height == image_heigth)) {
		height_in_blocks++;
	}
	int required_matrix_width = width_in_blocks * block_width;
	int required_matrix_heigth = height_in_blocks * block_height;

	unsigned char *data;
	data = (unsigned char*)calloc(required_matrix_width
		* required_matrix_heigth, sizeof(unsigned char));
	for (int i = 0; i < image_heigth; i++) {
		for (int j = 0; j < image_width; j++) {
			data[i*required_matrix_width + j] =
				image->data[i*image_width + j];
		}
	}

	free(image->data);
	image->data = data;
	image->pitch = required_matrix_width;
}

void show_iamge_part(PGMImage* image, const int width, const int height)
{
	int image_width = (image->pitch > 0) ? image->pitch : image->sizeX;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%4d", image->data[i * image_width + j]);
		}
		printf("\n");
	}
}

void add_frame(PGMImage* image, const int frame_size)
{
	int new_image_width = image->sizeX + frame_size * 2;
	int new_image_height = image->sizeY + frame_size * 2;
	unsigned char* result_data = calloc(new_image_height * new_image_width, sizeof(char));
	for (int i = 0; i < frame_size; i++) {
		int j = 0;
		for (; j < frame_size; j++) {
			result_data[i*new_image_width + j] = image->data[0];
		}
		for (; j < (image->sizeX + frame_size); j++) {
			result_data[i*new_image_width + j] = image->data[j - frame_size];
		}
		for (; j < (image->sizeX + 2 * frame_size); j++) {
			result_data[i*new_image_width + j] = image->data[image->sizeX - 1];
		}
	}
	for (int i = 0; i < image->sizeY; i++) {
		int j = 0;
		for (; j < frame_size; j++) {
			result_data[(i+frame_size)*new_image_width + j] 
				= image->data[i*image->sizeX];
		}
		for (; j < (image->sizeX + frame_size); j++) {
			result_data[(i + frame_size)*new_image_width + j]
				= image->data[i*image->sizeX + j - frame_size];
		}
		for (; j < new_image_width; j++) {
			result_data[(i + frame_size)*new_image_width + j] 
				= image->data[i*image->sizeX + image->sizeX - 1];
		}
	}
	for (int i = (image->sizeY + frame_size); i < (image->sizeY + 2*frame_size); i++) {
		int j = 0;
		for (; j < frame_size; j++) {
			result_data[i*new_image_width + j] 
				= image->data[(image->sizeY-1)*image->sizeX];
		}
		for (; j < (image->sizeX + frame_size); j++) {
			result_data[i*new_image_width + j] 
				= image->data[(image->sizeY - 1)*image->sizeX + j - frame_size];
		}
		for (; j < (image->sizeX + 2 * frame_size); j++) {
			result_data[i*new_image_width + j] 
				= image->data[(image->sizeY - 1)*image->sizeX + image->sizeX-1];
		}
	}
	image->data = result_data;
	image->frame_size = frame_size;
}

void write_pgm(PGMImage* image, const char *filename)
{
	FILE *fp;
	fp = fopen(filename, "wb");
	fprintf(fp, "P5\n");
	fprintf(fp, "# opencl\n");
	fprintf(fp, "%lu %lu\n", image->sizeX, image->sizeY);
	fprintf(fp, "255\n");
	if (image->pitch > 0) {
		for (int i = image->frame_size; i < image->sizeY + image->frame_size; i++) {
			fwrite(image->data + i * image->pitch + image->frame_size,
				sizeof(char),
				image->sizeX,
				fp);
		}
	}
	else {
		for (int i = image->frame_size; i < image->sizeY + image->frame_size; i++) {
			fwrite(image->data + i * (image->sizeX + 2 * image->frame_size) + image->frame_size,
				sizeof(char),
				image->sizeX,
				fp);
		}
	}
	fclose(fp);
}

void write_pgm_with_frame(PGMImage* image, const char *filename)
{

	FILE *fp;
	fp = fopen(filename, "wb");
	fprintf(fp, "P5\n");
	fprintf(fp, "# opencl\n");
	fprintf(fp, "%lu %lu\n",
		image->sizeX + 2 * image->frame_size,
		image->sizeY + 2 * image->frame_size);
	fprintf(fp, "255\n");
	if (image->pitch > 0) {
		for (int i = 0; i < (image->sizeY + 2 * image->frame_size); i++) {
			fwrite(image->data + i * image->pitch,
				sizeof(char),
				image->sizeX + 2 * image->frame_size,
				fp);
		}
	}
	else {
		fwrite(image->data, sizeof(char),
			(image->sizeX + 2 * image->frame_size)*(image->sizeY + 2 * image->frame_size),
			fp);
	}
	fclose(fp);
}