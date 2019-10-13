#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_UNIX_PATH        "./sockfd.dgram"
#define LISTEN_BACKLOG          5
#define print_usage(filename)   printf("usage : %s <server|client>\n", filename)
#define handle_error(msg)       do {\
                                    perror(msg);\
                                    return -1;\
                                } while(0);

// socket -> bind -> sendto/recvfrom -> close
int sock_server()
{
    unlink(SOCKET_UNIX_PATH);
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
        handle_error("failed to socket");

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCKET_UNIX_PATH
    };
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)))
        handle_error("failed to bind");

    char buf[1024] = {'a'};
    while (1) {
        sleep(1);
        if (sendto(sockfd, buf, 1, 0, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
            perror("failed to write");
            continue;
        }
        printf("data sent (%s)\n", buf);
        buf[0] = buf[0] + 1 > 'z' ? 'a' : buf[0] + 1;
    }
    
    close(sockfd);
    return 0;
}

// socket -> connect -> sendto/recvfrom -> close
int sock_client()
{
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
        handle_error("failed to socket");

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCKET_UNIX_PATH
    };
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)))
        handle_error("failed to connect");

    char buf[1024];
    while (1) {
        memset(buf, 0, sizeof(buf));
        sleep(1);
        int ret = recvfrom(sockfd, buf, 1, 0, NULL, NULL);
        if (ret == -1) {
            perror("failed to recvfrom");
            continue;
        } else {
            printf("recv size (%d)\n", ret);
        }
        printf("data recv (%s)\n", buf);
    }

    close(sockfd);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "server")) {
        if (sock_server())
            return -1;
    } else if (!strcmp(argv[1], "client")) {
        if (sock_client())
            return -1;
    } else {
        print_usage(argv[0]);
        return -1;
    }

    return 0;
}
