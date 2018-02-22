opts = -fopenmp -std=c11 -O3 ${d}

lab2: clean matrix2.s main.s
	gcc ${opts} matrix2.s main.s -o $@

matrix2.s:
	gcc ${opts} -S -I. matrix2.c -o $@

main.s:
	gcc ${opts} -S -I. main.c -o $@

# gcc -std=c11 -O3 -fopt-info-vec-all -I. main.c matrix2.c -o tmp

ompp:
	kinst-ompp gcc ${opts} -I. main.c matrix2.c -o ompp

.PHONY: clean run rerun

rerun: clean lab2 run bench_cache bench

run:
	./lab2

bench_cache: clean lab2 
	valgrind --tool=cachegrind ./lab2 cache	

bench: clean lab2
	valgrind --tool=cachegrind ./lab2 no-cache

clean:
	rm -f lab2
	rm -f matrix2.s
	rm -f main.s
