#include <stdio.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define SEM_NAME            "/semname"
#define print_usage(x)      printf("usage: %s <giver|getter|monitor|delete>\n", x);

// posix named : sem_open, sem_wait/trywait/timedwait, sem_post, sem_close, sem_unlink

int giver()
{
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("failed to sem_open");
        return -1;
    }

    printf("giver(%d) started.\n", getpid());

    while (1) {
        sleep(1);
        if (sem_post(sem) == 0)
            printf("giver(%d) gave.\n", getpid());
        else
            return -1;
    }

    return 0;
}

int getter()
{
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("failed to sem_open");
        return -1;
    }

    printf("getter(%d) started.\n", getpid());

    while (1) {
        usleep(200000);
        if (sem_trywait(sem) == 0)
            printf("getter(%d) gotten.\n", getpid());
        else
        {
            if (errno == EAGAIN)
                printf("getter(%d) says, \"give me more!\"\n", getpid());
            else
                return -1;
        }
    }

    return 0;
}

int monitor()
{
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("failed to sem_open");
        return -1;
    }

    printf("monitor(%d) started.\n", getpid());

    while (1) {
        usleep(200000);
        int value = 0;
        if (sem_getvalue(sem, &value) == -1) {
            perror("failed to sem_getvalue");
            return -1;
        }
        printf("monitor(%d) current semaphore(%d)\n", getpid(), value);
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    } else if (!strcmp(argv[1], "giver")) {
        if (giver() == -1)
            return -1;
    } else if (!strcmp(argv[1], "getter")) {
        if (getter() == -1)
            return -1;
    } else if (!strcmp(argv[1], "monitor")) {
        if (monitor() == -1)
            return -1;
    } else if (!strcmp(argv[1], "delete")) {
        if (sem_unlink(SEM_NAME) == -1) {
            perror("failed to sem_unlink");
            return -1;
        }
    } else {
        print_usage(argv[0]);
        return -1;
    }
    return 0;
}
