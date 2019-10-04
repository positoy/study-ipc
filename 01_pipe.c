#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// open pipe
// fork
// close unused fd
// each read and write

// write size < PIPE_BUF : atomic
// else : interleaved

int main(int argc, char** argv)
{
    int ptoc[2], ctop[2];
    char rdbuf[10];
    char wrchar;
    pid_t pid;
    int ret;

    if (pipe(ptoc) == -1 || pipe(ctop) == -1 || (pid = fork()) == -1) {
        perror("failed to pipe or fork");
        return -1;
    }

    if (pid == 0) {
        // child
        close(ctop[0]);
        close(ptoc[1]);
        wrchar = 'a';

        printf("[%d] child starts\n", getpid());
        while(1) {
            if (write(ctop[1], &wrchar, 1) == -1)
                perror("child failed to write");
            wrchar = (wrchar + 1 > 'z') ?  'a' : (wrchar + 1);

            sleep(1);

            memset(rdbuf, 0, sizeof(rdbuf));
            ret = read(ptoc[0], rdbuf, 10);
            if (ret == -1) perror("child failed to read");
            else printf("child read result(%s)\n", rdbuf);
        }

        close(ctop[1]);
        close(ptoc[0]);
    } else {
        // parent
        close(ctop[1]);
        close(ptoc[0]);
        wrchar = 'A';

        printf("[%d] parent starts\n", getpid());
        while(1) {
            if (write(ptoc[1], &wrchar, 1) == -1)
                perror("parent failed to write");
            wrchar = (wrchar + 1 > 'Z') ?  'A' : (wrchar + 1);

            sleep(1);

            memset(rdbuf, 0, sizeof(rdbuf));
            ret = read(ctop[0], rdbuf, 10);
            if (ret == -1) perror("parent failed to read");
            else printf("parent read result(%s)\n", rdbuf);
        }

        close(ctop[0]);
        close(ptoc[1]);
    }

    return 0;
}