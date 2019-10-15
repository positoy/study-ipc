#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>


#define print_usage(progname)       printf("usage : %s [monitor]\n", progname)

// file mapping : unrelated process 간 메모리 공유 가능 (file io를 이용하므로 성능저하)
// anonymous mapping : related process 간 메모리 공유 가능 (file io를 사용하지 않으므로 상대적으로 성능좋음) 

struct shm_data {
    pid_t pid;
    int count;
};

struct shm_data* init_shared_data() {
    struct shm_data* sharedData = mmap(NULL, sizeof(struct shm_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (MAP_FAILED == sharedData) {
        perror("failed to mmap");
        return NULL;
    }
    return sharedData;
}

int mmap_monitor(struct shm_data* sharedData)
{
    struct shm_data localData;

    while (1) {
        usleep(100000);
        if (memcmp(sharedData, &localData, sizeof(struct shm_data))) {
            memcpy(&localData, sharedData, sizeof(struct shm_data));
            printf("pid(%d), count(%d)\n", localData.pid, localData.count);
        }
    }
    return 0;
}

int mmap_writer(int index, struct shm_data* sharedData)
{
    int count = 0;
    sleep(index);
    sharedData->pid = getpid();
    sharedData->count = ++count;
    return 0;
}

int main(int argc, char** argv)
{
    pid_t pid;
    struct shm_data* sharedData = init_shared_data();

#define NUM_FORK        5
    for (int i=0; i<NUM_FORK; i++) {
        pid = fork();
        if (pid > 0) {
             /* parent */
        } else if (pid == 0) {
            /* child */
            if (mmap_writer(i, sharedData) == -1) {
                perror("failed to mmap_writer");
                return -1;
            }
            return 0;
        } else {
            /* error */
            perror("failed to fork");
            return -1;
        }
    }

    if (mmap_monitor(sharedData) == -1) {
        perror("failed to mmap_monitor");
        return -1;
    }

    for (int i=0; i<NUM_FORK; i++) {
        pid_t rpid = wait(NULL);
        printf("pid(%d) is end\n", rpid);
    }

    munmap(sharedData, sizeof(struct shm_data));
    return 0;
}
