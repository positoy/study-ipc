#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define FILEPATH            "./.lock.fcntl"
#define FILESIZE            1024
#define NUMSECTION          4
#define SECTIONSIZE         FILESIZE/NUMSECTION
#define print_usage(x)      printf("usage : %s <rd|wr> SECTION#\n", x)


void printLockStatus(struct flock *status)
{
    printf("l_type(%s), l_whence(%s), l_start(%ld), l_len(%ld), l_pid(%d)\n",
    status->l_type == F_RDLCK ? "F_RDLCK" :
    status->l_type == F_WRLCK ? "F_WRLCK" :
    status->l_type == F_WRLCK ? "F_UNLCK" : "UNKNOWN",
    status->l_whence == SEEK_SET ? "SEEK_SET" : 
    status->l_whence == SEEK_CUR ? "SEEK_CUR" : 
    status->l_whence == SEEK_END ? "SEEK_END" : "UNKNOWN",
    status->l_start, status->l_len, status->l_pid);
}

int main(int argc, char** argv)
{
    struct flock lock;
    int section = 0;
    memset(&lock, 0, sizeof(struct flock));

    if (argc == 3 && !strcmp("rd", argv[1])) {
        lock.l_type = F_RDLCK;
        section = SECTIONSIZE * atoi(argv[2]);
    } else if (argc == 3 && !strcmp("wr", argv[1])) {
        lock.l_type = F_WRLCK;
        section = SECTIONSIZE * atoi(argv[2]);
    } else {
        print_usage(argv[0]);
        return -1;
    }
    
    lock.l_whence = SEEK_SET;
    lock.l_start = SECTIONSIZE * section;
    lock.l_len = SECTIONSIZE;

    int fd = open(FILEPATH, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("failed to open");
        return -1;
    }
    if (ftruncate(fd, FILESIZE) == -1) {
        close(fd);
        perror("failed to ftruncate");
        return -1;
    }

    puts("trying to get the lock...");
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("failed to fcntl");
        close(fd);
        return -1;
    }
    printLockStatus(&lock);

    printf("got the lock. input enter :");
    getc(stdin);
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("failed to fcntl");
        close(fd);
        return -1;
    }
    printLockStatus(&lock);

    close(fd);
    return 0;
}