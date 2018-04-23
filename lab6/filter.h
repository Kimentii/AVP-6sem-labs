#ifndef FILTER_H
#define FILTER_H

#include <sys/types.h>

#define FILTER_TYPE_BLUR 1

typedef float ftype;

ftype* get_filter(int h, int w, int type);
void normalize_filter(ftype* filter, int h, int w, ftype sum);

#endif
