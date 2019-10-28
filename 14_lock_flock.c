#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILEPATH            "./.lock.flock"
#define print_usage(x)      printf("usage : %s <ex|sh>\n", x)

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    int fd = open(FILEPATH, O_CREAT, 0644);
    if (fd == -1) {
        perror("failed to open");
        return -1;
    }

    puts("trying to get the lock...");
    if (!strcmp("sh", argv[1])) {
        flock(fd, LOCK_SH);
    } else if (!strcmp("ex", argv[1])) {
        flock(fd, LOCK_EX);
    } else {
        print_usage(argv[0]);
        close(fd);
        return -1;
    }
    printf("got the lock. input enter :");
    getc(stdin);
    flock(fd, LOCK_UN);

    return 0;
}