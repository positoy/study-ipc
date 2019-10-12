#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_FILEPATH   "./fifofd"
#define handle_error(msg)       do{\
                                    perror(msg);\
                                    return -1;\
                                } while(0)
#define print_usage(filepath)   printf("usage: %s <send|recv>\n", filepath)

int fifo_send()
{
    unlink(FIFO_FILEPATH);
    if (mkfifo(FIFO_FILEPATH, 0666) == -1)
        handle_error("failed to mkfifo");

    int fd = open(FIFO_FILEPATH, O_WRONLY);
    if (fd == -1)
        handle_error("failed to open");

    puts("client connected.");

    char wrdata = 'a';
    while (1) {
        sleep(1);
        if (write(fd, &wrdata, 1) == -1)
            handle_error("failed to write");

        printf("sent (%c)\n", wrdata);
        wrdata = (wrdata+1) > 'z' ? 'a' : wrdata+1;
    }

    close(fd);
    return 0;
}

int fifo_recv()
{
    int fd = open(FIFO_FILEPATH, O_RDONLY);
    if (fd == -1)
        handle_error("failed to open");

    puts("connected to a server.");

    char rdbuf[10];
    while (1) {
        memset(rdbuf, 0, sizeof(rdbuf));
        if (read(fd, rdbuf, sizeof(rdbuf)) == -1)
            handle_error("failed to read");

        printf("recv (%s)\n", rdbuf);
    }

    close(fd);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "send")) {
        if (fifo_send())
            return -1;
    } else if (!strcmp(argv[1], "recv")) {
        if (fifo_recv())
            return -1;
    } else {
        print_usage(argv[1]);
        return -1;
    }

    return 0;
}
