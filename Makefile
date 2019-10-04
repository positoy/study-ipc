all:	pipe.out

pipe.out:	01_pipe.c
	gcc -g -Wall 01_pipe.c -o pipe.out

clean:
	rm -rf *.out