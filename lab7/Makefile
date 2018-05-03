opencl_flags := -Wno-deprecated-declarations
src_files := main.c cl_api.c matrix.c filter.c ppm.c cpu_filter.c time_measure.c

main: clean main.c cl_api.c matrix.c
	gcc ${src_files} -o $@ ${opencl_flags} -I "/c/Program Files (x86)/AMD APP SDK/3.0/include" -L "/c/Program Files (x86)/AMD APP SDK/3.0/lib/x86_64" -lOpenCL

.PHONY: clean

clean:
	rm -f main
