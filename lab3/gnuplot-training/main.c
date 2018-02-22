#include <stdio.h>

#include "gnuplot_i.h"

int main()
{
	double y[3];
	double x[3];
	x[0] = 1.5f;
	y[0] = 1.5f;
	x[1] = 2.0f;
	y[1] = 3.0f;
	x[2] = 2.5f;
	y[2] = 2.0f;
	
	gnuplot_ctrl *h = gnuplot_init();
	gnuplot_setstyle(h, "linespoints");
	gnuplot_plot_xy(h, x, y, 3, "data");
	getchar();
	gnuplot_close(h);	
	return 0;
}
