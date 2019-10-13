all:	 pipe.out fifo.out mq_sysv.out mq_posix.out socket_stream.out

pipe.out:	01_pipe.c
	gcc -g -Wall 01_pipe.c -o pipe.out

fifo.out:	02_fifo.c
	gcc -g -Wall 02_fifo.c -o fifo.out

mq_sysv.out:	03_mq_sysv.c
	gcc -g -Wall 03_mq_sysv.c -o mq_sysv.out

mq_posix.out:	04_mq_posix.c
	gcc -g -Wall 04_mq_posix.c -o mq_posix.out -lrt

socket_stream.out:	05_socket_stream.c
	gcc -g -Wall 05_socket_stream.c -o socket_stream.out

clean:
	rm -rf *.out fifofd sockfd.*
