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
PGMImage* mock_pgm(int h, int w);
void write_pgm(PGMImage* image, const char *filename);

#endif
