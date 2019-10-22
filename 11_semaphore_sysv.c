#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// sysV : semget, semop, semctl
// posix unnamed : sem_init, sem_wait/trywait/timedwait, sem_post, sem_destroy, 
// posix named : sem_open, sem_wait/trywait/timedwait, sem_post, sem_close, sem_unlink

#define PATHNAME            "/tmp"
#define PROJ_ID             'd'
#define NSEMS               2
#define print_usage(x)      printf("usage: %s <show|get num|release num|wait0 num>\n", x)

static int init_semaphore()
{
	key_t key;
	int semid;

	key = ftok(PATHNAME, PROJ_ID);
	if (key == -1) {
		perror("failed to ftok");
		return -1;
	}

	semid = semget(key, NSEMS, IPC_CREAT | 0644);
	if (semid == -1) {
		perror("failed to semget");
		return -1;
	}

	return semid;
}

static int sem_init(int semid)
{
	semctl(semid, 0, SETVAL, 1);	/* binary semaphore */
	semctl(semid, 1, SETVAL, 3);	/* counting semaphore */
	return 0;
}

static int sem_show(int semid)
{
	for (int i = 0; i < NSEMS; i++)
		printf("semaphore[%d] = %d\n", i, semctl(semid, i, GETVAL));

	return 0;
}

static int sem_get(int semid, int num)
{
	struct sembuf sb = {
        .sem_num = num,
        .sem_op = -1
    };

    if (semop(semid, &sb, 1) == -1) {
        perror("failed to semop");
        return -1;
    }

    return 0;
}

int sem_release(int semid, int num)
{
	struct sembuf sb = {
        .sem_num = num,
        .sem_op = 1
    };

    if (semop(semid, &sb, 1) == -1) {
        perror("failed to semop");
        return -1;
    }

    return 0;
}

int sem_wait0(int semid, int num)
{
    struct sembuf sb = {
        .sem_num = num,
        .sem_op = 0
    };

    if (semop(semid, &sb, 1) == -1) {
        perror("failed to semop");
        return -1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return -1;
    }

    int semid = init_semaphore();
    if (semid == -1) {
        perror("failed to init_semaphore");
        return -1;
    }

    if (argc == 2 && !strcmp(argv[1], "init")) {
        if (sem_init(semid) == -1)
            return -1;
    } else if (argc == 2 && !strcmp(argv[1], "show")) {
        if (sem_show(semid) == -1)
            return -1;
    } else if (argc == 3 && !strcmp(argv[1], "get")) {
        if (sem_get(semid, atoi(argv[2])) == -1)
            return -1;
    } else if (argc == 3 && !strcmp(argv[1], "release")) {
        if (sem_release(semid, atoi(argv[2])) == -1)
            return -1;
    } else if (!strcmp(argv[1], "wait0")){
        if (sem_wait0(semid, atoi(argv[2])) == -1)
            return -1;
    } else {
        print_usage(argv[0]);
        return -1;
    }
    return 0;
}
