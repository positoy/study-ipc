#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

const char FIFO_FILEPATH[] = "./fifofd"; 

#if defined(SERVER)
int main()
{
    unlink(FIFO_FILEPATH);
    if (mkfifo(FIFO_FILEPATH, 0644) == -1)
        perror("failed to mkfifo");

    int fd = open(FIFO_FILEPATH, O_WRONLY);

    puts("client connected.");

    char wrdata = 'a';

    while (1) {
        sleep(1);
        if (write(fd, &wrdata, 1) == -1)
            perror("failed to write");
        else
            printf("server write (%c)", wrdata);
        wrdata = (wrdata+1) > 'z' ? 'a' : wrdata+1;
    }

    close(fd);
    
}
#elif defined(CLIENT)
int main()
{
    int fd = open(FIFO_FILEPATH, O_RDONLY);

    puts("connected to a server.");

    char rdbuf[10];
    while (1) {
        sleep(1);
        memset(rdbuf, 0, sizeof(rdbuf));
        if (read(fd, rdbuf, sizeof(rdbuf)) == -1)
            perror("failed to read");
        else
            printf("client read (%s)\n", rdbuf);
    }

    close(fd);
}
#endif

