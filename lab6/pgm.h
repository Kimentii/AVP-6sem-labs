#ifndef PGM
#define PGM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int sizeX, sizeY;
	unsigned char *data;
} PGMImage;

PGMImage* read_pgm(const char *filename);

void write_pgm(PGMImage* image, const char *filename);

#endif
