#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int lockf(int fd, int cmd, off_t len);

#define FILEPATH            ".lock.lockf"
#define FILESIZE            1024
#define SECTION_NUMBER      4
#define SECTION_LENGTH      (FILESIZE/SECTION_NUMBER)
#define print_usage(x)      printf("usage : %s <lock|nonblock> SECTION#\n", x)

int main(int argc, char** argv)
{
    int cmd, section;

    if (argc == 3 && !strcmp("lock", argv[1])) {
        cmd = F_LOCK;
        section = atoi(argv[2]);
    }
    else if (argc == 3 && !strcmp("nonblock", argv[1])) {
        cmd = F_TLOCK;
        section = atoi(argv[2]);
    }
    else {
        print_usage(argv[0]);
        return -1;
    }

    int fd = open(FILEPATH, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("failed to open");
        return -1;
    }
    if (ftruncate(fd, FILESIZE) == -1) {
        perror("failed to ftruncate");
        close(fd);
        return -1;
    }
    if (lseek(fd, SECTION_LENGTH * section, SEEK_SET) == -1) {
        perror("failed to lseek");
        close(fd);
        return -1;
    }

    puts("trying to get the lock...");
    if (lockf(fd ,cmd, SECTION_LENGTH) == -1) {
        perror("failed to lockf");
        close(fd);
        return -1;
    }

    printf("got the lock. input enter :");
    getc(stdin);

    cmd = F_UNLCK;
    if (lockf(fd ,cmd, SECTION_LENGTH) == -1) {
        perror("failed to lockf");
        close(fd);
        return -1;
    }

    return 0;
}
