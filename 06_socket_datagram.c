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
    int sockfd;
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCKET_UNIX_PATH
    };
    char buf[1024];

    unlink(SOCKET_UNIX_PATH);
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
        handle_error("failed to socket");

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)))
        handle_error("failed to bind");

    while (1) {
        memset(buf, 0, sizeof(buf));
        if (recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL) == -1) {
            perror("failed to recvfrom");
            continue;
        }
        printf("recvfrom (%s)\n", buf);
    }
    
    close(sockfd);
    return 0;
}

// socket -> sendto/recvfrom -> close
int sock_client()
{
    int sockfd;
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCKET_UNIX_PATH
    };
    char data = 'a';
    
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
        handle_error("failed to socket");

    while (1) {
        sleep(1);
        if (sendto(sockfd, &data, 1, 0, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
            perror("failed to sendto");
            continue;
        }
        printf("sendto (%c)\n", data);
        data = data + 1 > 'z' ? 'a' : data + 1;
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
