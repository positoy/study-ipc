#include <stdio.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

// posix unnamed : sem_init, sem_wait/trywait/timedwait, sem_post, sem_destroy, 
// posix named : sem_open, sem_wait/trywait/timedwait, sem_post, sem_close, sem_unlink


sem_t*
init_anonymous_semaphore()
{
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (MAP_FAILED == sem) {
        perror("failed to mmap");
        return (void*)-1;
    }

    if (sem_init(sem, 0, 0) == -1) {
        perror("failed to sem_init");
        return (void*)-1;
    }

    return sem;
}

int giver(sem_t* sem)
{
    puts("giver starts");

    while (1) {
        sleep(1);
        if (sem_post(sem) == 0)
            puts("giver gave.");
        else
            return -1;
    }

    return 0;
}

int getter(sem_t* sem)
{
    puts("getter starts");

    while (1) {
        usleep(200000);
        if (sem_trywait(sem) == 0)
            puts("getter gotten.");
        else
        {
            if (errno == EAGAIN)
                puts("getter says, \"give me more!\"");
            else
                return -1;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    sem_t* sem = init_anonymous_semaphore();

    if (fork()) /* parent */
        giver(sem);
    else        /* child */
        getter(sem);

    return 0;
}
