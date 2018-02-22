#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>

#define M_HEIGHT	100
#define M_WIDTH 	100
#define M_ELEMENTS	(M_HEIGHT * M_WIDTH)
#define CELL_HEIGHT	32
#define CELL_WIDTH 	32
#define CELL_ELEMENTS	(CELL_HEIGHT * CELL_WIDTH)

#define EXIT_ERROR	1

typedef double mtype;

mtype** matrix_init(bool);

mtype** matric_static_init();

void matrix_show(mtype**);

mtype** matrix_multiply(mtype**, mtype**);

mtype** matrix_multiply_asm(mtype**, mtype**);

void matrix_compare(mtype**, mtype**);

void matrix_destroy();

#endif
