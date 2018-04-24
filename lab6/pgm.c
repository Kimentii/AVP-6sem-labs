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
	result->pitch = 0;
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

	return result;
}

void resize_image(PGMImage* image, const int block_width, const int block_height)
{
	int width_in_blocks = image->sizeX / block_width;
	if (!(width_in_blocks * block_width == image->sizeX)) {
		width_in_blocks++;
	}
	int height_in_blocks = image->sizeY / block_height;
	if (!(height_in_blocks * block_height == image->sizeY)) {
		height_in_blocks++;
	}
	int required_matrix_width = width_in_blocks * block_width;
	int required_matrix_heigth = height_in_blocks * block_height;

	unsigned char *data;
	data = (unsigned char*)calloc(required_matrix_width
		* required_matrix_heigth, sizeof(unsigned char));
	for (int i = 0; i < image->sizeY; i++) {
		for (int j = 0; j < image->sizeX; j++) {
			data[i*required_matrix_width + j] =
				image->data[i*image->sizeX + j];
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

void write_pgm(PGMImage* image, const char *filename)
{
	FILE *fp;
	fp = fopen(filename, "wb");
	fprintf(fp, "P5\n");
	fprintf(fp, "# opencl\n");
	fprintf(fp, "%lu %lu\n", image->sizeX, image->sizeY);
	fprintf(fp, "255\n");
	if (image->pitch > 0) {
		for (int i = 0; i < image->sizeY; i++) {
			fwrite(image->data + i * image->pitch, sizeof(char), image->sizeX, fp);
		}
	} else {
		fwrite(image->data, sizeof(char), image->sizeX*image->sizeY, fp);
	}
	fclose(fp);
}
