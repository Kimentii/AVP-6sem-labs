#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

#include "gnuplot_i.h"
#include "matrix.h"

#define N 						1000000
#define BYTES_IN_KB				1024
#define BYTES_IN_MB				1024 * 1024
#define MAX_CACHE_WAY			20
#define CACHE_SIZE_KB			64
#define DELIMETER_SIZE_BYTES	BYTES_IN_MB - STR_SIZE_BYTES
#define CELL_SIZE_BYTES			64
#define CELL_PER_STR			(CACHE_SIZE_KB * BYTES_IN_KB) / (MAX_CACHE_WAY * CELL_SIZE_BYTES)
#define STR_SIZE_BYTES 			CELL_PER_STR * CELL_SIZE_BYTES

struct unit 
{
	u_int64_t space_taker[7];
	struct unit* next;
};

struct unit *mass;

struct unit* prepare_mass_to_way(int way)
{
	struct unit *mass = (struct unit*) malloc(MAX_CACHE_WAY * BYTES_IN_MB);
	for (int i = 0; i < CELL_PER_STR; i++) {
		for (int j = 0; j < way; j++) {
			struct unit* next;
			if (j < way - 1) {
				next = (struct unit*)((char*)mass + i * CELL_SIZE_BYTES + ((j + 1) * BYTES_IN_MB));
			} else {
				if (i < CELL_PER_STR - 1) {
					next = (struct unit*)((char*)mass + (i + 1) * CELL_SIZE_BYTES);					
				} else {
					next = mass;
				}
			}
			((struct unit*)((char*)mass  + i * CELL_SIZE_BYTES + (j * BYTES_IN_MB)))->next = next;

		}
	}
	return mass;
}

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile ("rdtsc" : "=A" (x));
    return x;
}

double measure_way_time(struct unit* mass, int way)
{
	struct unit *tmp = mass;
	unsigned long long start_time = rdtsc();
	for (int k = 0; k < N; k++) {
		tmp = tmp->next;
	}
	unsigned long long end_time = rdtsc();
	unsigned long long result = end_time - start_time;
	return (double)(result / N);
}

int main(int argc, char *argv[]) {
	struct unit* mass;
	double x[MAX_CACHE_WAY];
	double y[MAX_CACHE_WAY];
	for (int i = 1; i < MAX_CACHE_WAY; i++) {
			x[i] = (double)i;
			mass = prepare_mass_to_way(i);
			y[i] = measure_way_time(mass, i);
			printf("%d-%F\n", i, y[i]);
			free(mass);
	}
	gnuplot_ctrl *h = gnuplot_init();
	gnuplot_setstyle(h, "linespoints");
	gnuplot_plot_xy(h, x, y, MAX_CACHE_WAY, "avg time clocks");
	getchar();
	gnuplot_close(h);
	return 0;
}
