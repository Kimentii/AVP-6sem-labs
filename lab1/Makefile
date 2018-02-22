opts = -O3 -fopt-info-vec-optimized ${d} -std=c11

lab1: matrix.s main.s
	gcc ${opts} $^ -o $@

matrix.s:
	gcc ${opts} -S -I. matrix.c -o $@

main.s:
	gcc ${opts} -S -I. main.c -o $@

# gcc -std=c11 -O3 -fopt-info-vec-all -I. main.c matrix.c -o tmp

.PHONY: clean

clean:
	rm lab1
	rm matrix.s
	rm main.s
