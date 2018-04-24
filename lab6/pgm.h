#ifndef PGM
#define PGM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	size_t sizeX, sizeY, pitch;
	unsigned char *data;
} PGMImage;

PGMImage* read_pgm(const char *filename);

void resize_image(PGMImage* image, const int block_width, const int block_height);

void show_iamge_part(PGMImage* image, const int width, const int height);

void write_pgm(PGMImage* image, const char *filename);

#endif
