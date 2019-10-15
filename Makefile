all:	 pipe.out fifo.out mq_sysv.out mq_posix.out socket_stream.out socket_datagram.out mmap_file.out mmap_anonymous.out

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

socket_datagram.out:	06_socket_datagram.c
	gcc -g -Wall 06_socket_datagram.c -o socket_datagram.out

mmap_file.out:	07_mmap_file.c 
	gcc -g -Wall 07_mmap_file.c -o mmap_file.out

mmap_anonymous.out:	08_mmap_anonymous.c 
	gcc -g -Wall 08_mmap_anonymous.c -o mmap_anonymous.out

clean:
	rm -rf *.out fifofd sockfd.* shared_file
