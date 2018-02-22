#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>

/* Kirill's proc: 4 threads, cache3 - 3MB, cache2 - 256KB,
cache1(data) - 2*32KB.
*/

/*
Mike proc: 
* 2 cores
* L1i — 64KB
* L1d — 64KB
* L2 — 1024KB 
*/

#ifndef M_SIZE
	#define M_SIZE 512
#endif
#ifndef B_SIZE
	#define B_SIZE 16
#endif

#define M_HEIGHT	M_SIZE
#define M_WIDTH 	M_SIZE

#define B_HEIGHT 	B_SIZE
#define B_WIDTH		B_SIZE

#define M_ELEMENTS	(M_HEIGHT * M_WIDTH)
#define B_ELEMENTS	(B_HEIGHT * B_WIDTH)

#define B_COUNT		(M_ELEMENTS / B_ELEMENTS)
#define B_IN_HEIGHT	(M_HEIGHT / B_HEIGHT)
#define B_IN_WIDTH	(M_WIDTH / B_WIDTH)

#define EXIT_ERROR	1

typedef double mtype;

mtype* matrix_init(bool);

mtype* matric_static_init();

void matrix_show(mtype*);

mtype* matrix_multiply(mtype*, mtype*);

mtype* matrix_multiply_cache(mtype*, mtype*);

mtype* matrix_multiply_openmp(mtype*, mtype*);

void matrix_destroy();

void matrix_compare(mtype*, mtype*);
#endif
