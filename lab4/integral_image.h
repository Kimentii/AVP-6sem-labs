#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define L 100

void fill_random(int *src, int length);

void integral_image(int *src, int *result, int n, int m);

void integral_image_with_secret(int *src, int *result, int n, int m);

void print_matrix(int *m, int width, int length);

void matrix_compare(int* a, int* b, int size);
