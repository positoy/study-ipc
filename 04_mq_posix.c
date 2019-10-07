#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>

/*
watch -n 1 -d cat /dev/mqueue/..
*/

/*
mq_open, mq_close, mq_unlink
mq_send, mq_receive
mq_setattr, mq_getattr, mq_notify (singal, thread)
*/

#define MESSAGE_QUEUE_NAME  "/mq_posix"

void print_usage(char* filename)
{
    printf("usage : %s <send #|recv>\n", filename);
}

mqd_t mq_init()
{
    mqd_t mqdes = mq_open(MESSAGE_QUEUE_NAME, O_RDWR | O_CREAT, 0644, NULL);
    if (mqdes == -1) {
        perror("failed to mq_open");
        return -1;
    }

    return mqdes;
}

int server_send(unsigned long msg_prio)
{
    char mbuf[10240];
    mqd_t mqdes = mq_init();
    if (mqdes == -1)
        return -1;

    memset(mbuf, 0, sizeof(mbuf));
    snprintf(mbuf, sizeof(mbuf), "hello world from %d", getpid());
    if (mq_send(mqdes, mbuf, strlen(mbuf)+1, msg_prio) == -1) {
        perror("failed to mq_send");
        return -1;
    }

    mq_close(mqdes);
    return 0;
}

int client_recv()
{
    mqd_t mqdes;
    unsigned int msg_prio;
    char* mbuf;

    mqdes = mq_init();
    if (mqdes == -1)
        return -1;

    struct mq_attr attr;
    if (mq_getattr(mqdes, &attr) == -1) {
        perror("failed to mq_getattr");
        return -1;
    }
    // printf(" mq_flags(%ld) mq_maxmsg(%ld) mq_msgsize(%ld) mq_curmsgs(%ld)\n", attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    mbuf = (void*)malloc(attr.mq_msgsize);
    if (mq_receive(mqdes, mbuf, attr.mq_msgsize, &msg_prio) == -1) {
        perror("failed to mq_receive");
        return -1;
    }
    printf("receieve : message(%s), priority(%d)\n", mbuf, msg_prio);
    free(mbuf);

    mq_close(mqdes);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "send")) {
        if (argc != 3) {
            print_usage(argv[0]);
            return -1;
        }
        if (server_send(atoi(argv[2]))) return -1;

    } else if (!strcmp(argv[1], "recv")) {
        if (client_recv()) return -1;

    } else {
        print_usage(argv[0]);
        return -1;
    }
}