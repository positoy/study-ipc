#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#define SHARED_FILE_NAME            "/shared_file"
#define print_usage(progname)       printf("usage : %s [monitor]\n", progname)

// sysv : shmget, ftok, shmat, shmdt, shmctl (ipcs, ipcrm)
// posix : shm_open, ftruncate, mmap, munmap, shm_unlinn / shm_open, fstat, mmap, munmap, shm_unlink
// --> creates virtual memory file in /tmp/dev

struct shm_data {
    pid_t pid;
    int count;
};

int mmap_monitor()
{
    int fd;
    struct shm_data* sharedData;
    struct shm_data localData;
    
    fd = shm_open(SHARED_FILE_NAME, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("failed to shm_open");
        return -1;
    }

    if (ftruncate(fd, sizeof(struct shm_data)) == -1) {
        perror("failed to ftruncate");
        close(fd);
        return -1;
    }

    sharedData = mmap(NULL, sizeof(struct shm_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == sharedData) {
        perror("failed to mmap");
        close(fd);
        return -1;
    }

    memset(&localData, 0, sizeof(struct shm_data));
    while (1) {
        sleep(1);
        if (memcmp(sharedData, &localData, sizeof(struct shm_data))) {
            memcpy(&localData, sharedData, sizeof(struct shm_data));
            printf("pid(%d), count(%d)\n", localData.pid, localData.count);
        }
    }

    munmap(sharedData, sizeof(struct shm_data));
    shm_unlink(SHARED_FILE_NAME);
    return 0;
}

int mmap_writer()
{
    int fd;
    struct shm_data* sharedData;
    
    fd = shm_open(SHARED_FILE_NAME, O_RDWR, 0644);
    if (fd == -1) {
        perror("failed to shm_open");
        return -1;
    }

    sharedData = mmap(NULL, sizeof(struct shm_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == sharedData) {
        perror("failed to mmap");
        close(fd);
        return -1;
    }

    int count = 0;
    while (1)
    {
        sleep(1);
        sharedData->pid = getpid();
        sharedData->count = ++count;
    }

    munmap(sharedData, sizeof(struct shm_data));
    shm_unlink(SHARED_FILE_NAME);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc >= 2 && !strcmp(argv[1], "monitor")) {
        if (mmap_monitor())
            return -1;
    } else if (argc == 1) {
        if(mmap_writer())
            return -1;
    } else {
        print_usage(argv[0]);
        return -1;
    }

    return 0;
}
