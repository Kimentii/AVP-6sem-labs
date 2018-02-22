opts = -std=c11

lab3: clean main.c
	gcc ${opts} main.c gnuplot_i.o -o $@


.PHONY: clean run rerun

rerun: lab3 run 

run:
	./lab3

clean:
	rm -f lab3
