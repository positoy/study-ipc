all:	pipe.out fifo_server.out fifo_client.out mq_sysv.out 

pipe.out:	01_pipe.c
	gcc -g -Wall 01_pipe.c -o pipe.out

fifo_server.out:	02_fifo.c
	gcc -g -Wall 02_fifo.c -o fifo_server.out -DSERVER

fifo_client.out:	02_fifo.c
	gcc -g -Wall 02_fifo.c -o fifo_client.out -DCLIENT

mq_sysv.out:	03_mq_sysv.c
	gcc -g -Wall 03_mq_sysv.c -o mq_sysv.out

clean:
	rm -rf *.out fifofd
