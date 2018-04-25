#ifndef PGM
#define PGM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	size_t sizeX, sizeY, pitch, frame_size;
	unsigned char *data;
} PGMImage;

PGMImage* read_pgm(const char *filename);

PGMImage* mock_pgm(int w, int h);

void resize_image(PGMImage* image, const int block_width, const int block_height);

void show_iamge_part(PGMImage* image, const int width, const int height);

void add_frame(PGMImage* image, const int frame_size);

void write_pgm(PGMImage* image, const char *filename);

void write_pgm_with_frame(PGMImage* image, const char *filename);

#endif
