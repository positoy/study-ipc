all:	 pipe.out fifo.out mq_sysv.out mq_posix.out socket_stream.out socket_datagram.out mmap_file.out mmap_anonymous.out shm_sysv.out shm_posix.out sem_sysv.out sem_posix_unnamed.out sem_posix_naemd.out lock_flock.out lock_fcntl.out lock_lockf.out

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

shm_sysv.out:	09_shm_sysv.c 
	gcc -g -Wall 09_shm_sysv.c -o shm_sysv.out -lrt

shm_posix.out:	10_shm_posix.c 
	gcc -g -Wall 10_shm_posix.c -o shm_posix.out -lrt

sem_sysv.out:	11_semaphore_sysv.c 
	gcc -g -Wall 11_semaphore_sysv.c -o sem_sysv.out

sem_posix_unnamed.out: 12_semaphore_posix_unnamed.c 
	gcc -g -Wall 12_semaphore_posix_unnamed.c -o sem_posix_unnamed.out -lpthread

sem_posix_naemd.out: 13_semaphore_posix_named.c
	gcc -g -Wall 13_semaphore_posix_named.c -o sem_posix_named.out -lpthread

lock_flock.out: 14_lock_flock.c 
	gcc -g -Wall 14_lock_flock.c -o lock_flock.out

lock_fcntl.out: 15_lock_fcntl.c 
	gcc -g -Wall 15_lock_fcntl.c -o lock_fcntl.out

lock_lockf.out: 16_lock_lockf.c 
	gcc -g -Wall 16_lock_lockf.c -o lock_lockf.out

clean:
	rm -rf *.out fifofd sockfd.* shared_file .lock*
