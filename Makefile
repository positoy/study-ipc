all:	pipe.out fifo_server.out fifo_client.out

pipe.out:	01_pipe.c
	gcc -g -Wall 01_pipe.c -o pipe.out

fifo_server.out:	02_fifo.c
	gcc -g -Wall 02_fifo.c -o fifo_server.out -DSERVER

fifo_client.out:	02_fifo.c
	gcc -g -Wall 02_fifo.c -o fifo_client.out -DCLIENT

clean:
	rm -rf *.out fifofd
