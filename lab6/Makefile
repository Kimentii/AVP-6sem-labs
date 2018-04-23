opencl_flags := -Wno-deprecated-declarations
src_files := main.c cl_api.c matrix.c filter.c

main: clean main.c cl_api.c matrix.c
	gcc ${src_files} -o $@ ${opencl_flags} -lOpenCL

.PHONY: clean

clean:
	rm -f main
