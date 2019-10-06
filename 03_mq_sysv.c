#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
    
#define KEY_PATH_NAME   "/proc"
#define KEY_PROJECT_ID  'a'
/*
# ipcs

ftok, msgget, msgctl
msgsnd, msgrcv
* message type 송신은 posix 미지원

*/
struct msgbuf {
    long mtype;
#define MTEXT_SIZE  64
    char mtext[MTEXT_SIZE];
};

int init_msg()
{
    key_t msqkey;
    int msqid;

    msqkey = ftok(KEY_PATH_NAME, KEY_PROJECT_ID);
    if (msqkey == -1) {
        perror("failed to ftok");
        return -1;
    }

    puts("ftok successful");

    msqid = msgget(msqkey, 0644 | IPC_CREAT);
    if (msqid == -1) {
        perror("failed to msqget");
        return -1;
    }

    puts("msgget successful");

    return msqid;
}

int send_message()
{
    int msqid;
    struct msgbuf mbuf;

    msqid = init_msg();
    if (msqid == -1) {
        printf("failed to init_msg\n");
        return -1;
    }

    memset(&mbuf, 0, sizeof(struct msgbuf));
    mbuf.mtype = 2;
    strncpy((char*)mbuf.mtext, "hello world!", sizeof(mbuf.mtext)-1);

    if (msgsnd(msqid, &mbuf, sizeof(mbuf.mtext), 0) == -1) {
        perror("failed to msgsnd");
        return -1;
    }

    return 0;
}

int recv_message()
{
    int msqid;
    struct msgbuf mbuf;

    msqid = init_msg();
    if (msqid == -1) {
        printf("failed to init_msg\n");
        return -1;
    }

    memset(&mbuf, 0, sizeof(struct msgbuf));

    if (msgrcv(msqid, &mbuf, sizeof(mbuf.mtext), 2, 0) == -1) {
        perror("failed to msgsnd");
        return -1;
    }

    printf("recv result : %s\n", mbuf.mtext);

    return 0;
}

void print_usage(const char* filename)
{
    printf("usage : %s <server|client>\n", filename);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "server")) {
        if (send_message() == -1)
            return -1;
            
    } else if (!strcmp(argv[1], "client")) {
        if (recv_message() == -1)
            return -1;

    } else {
        print_usage(argv[0]);
        return -1;
    }

}