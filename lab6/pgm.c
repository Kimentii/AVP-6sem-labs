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

	return result;
}

void write_pgm(PGMImage* image, const char *filename)
{
	FILE *fp;
	fp = fopen(filename, "wb");
	fprintf(fp, "P5\n");
	fprintf(fp, "# opencl\n");
	fprintf(fp, "%lu %lu\n", image->sizeX, image->sizeY);
	fprintf(fp, "255\n");
	fwrite(image->data, sizeof(char), image->sizeX*image->sizeY, fp);
	fclose(fp);

}
