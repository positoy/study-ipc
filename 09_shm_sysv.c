#include <string.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/procfs.h>
#include <unistd.h>

#define SHARED_MEMORY_FILEPATH      "shm_sysv_shared"
#define SHARED_MEMORY_PROJECTID     'a'
#define ROUNDUP(x)                  (((x) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))
// sysv : ftok, shmget, shmat, shmdt, shmctl (ipcs, ipcrms)
// posix : shm_open, ftruncate, mmap, munmap, shm_unlinn / shm_open, fstat, mmap, munmap, shm_unlink

struct shm_data {
    pid_t pid;
    int count;
};

int shm_sysv_monitor(key_t key)
{
    int shmid, size;
    void* shmptr;
    struct shm_data localData;

    size = ROUNDUP(sizeof(struct shm_data));
    shmid = shmget(key, size, IPC_CREAT | 0644);
    if (shmid == -1) {
        perror("failed to shmget");
        return -1;
    }

    shmptr = shmat(shmid, NULL, 0);
    if (shmptr == (void*)-1) {
        perror("failed to shmat");
        return -1;
    }
    memset(&localData, 0, sizeof(struct shm_data));

    while (1) {
        sleep(1);
        if (memcmp(&localData, shmptr, sizeof(struct shm_data))) {
            memcpy(&localData, shmptr, sizeof(struct shm_data));
            printf("pid(%d), count(%d)\n", localData.pid, localData.count);
        }
    }

    shmdt(shmptr);
    return 0;
}

int shm_sysv_writer(key_t key)
{
    int size;
    int shmid;
    struct shm_data* sharedData;

    size = ROUNDUP(sizeof(struct shm_data));
    shmid = shmget(key, size, 0644);
    if (shmid == -1) {
        perror("failed to shmget");
        return -1;
    }

    sharedData = (struct shm_data*)shmat(shmid, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("failed to shmat");
        return -1;
    }

    int count = 0;
    while (1)
    {
        sleep(1);
        sharedData->pid = getpid();
        sharedData->count = ++count;
    }

    shmdt(sharedData);
    return 0;
}

int main(int argc, char** argv)
{
    key_t key = ftok(SHARED_MEMORY_FILEPATH, SHARED_MEMORY_PROJECTID);
    if (key == -1) {
        perror("failed to ftok");
        return -1;
    }

    if (argc == 2 && !strcmp(argv[1], "monitor")) {
        if (shm_sysv_monitor(key) == -1)
            return -1;
    } else if (argc == 1) {
        if (shm_sysv_writer(key) == -1)
            return -1;
    } else {
        printf("usage : %s [monitor]\n", argv[0]);
        return -1;
    }
    return 0;
}
