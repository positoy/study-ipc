#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_UNIX_PATH        "./sockfd.stream"
#define LISTEN_BACKLOG          5
#define print_usage(filename)   printf("usage : %s <server|client>\n", filename)
#define handle_error(msg)       do {\
                                    perror(msg);\
                                    return -1;\
                                } while(0);

int write_stream(int sockfd, char buff[], size_t len) {
    int ret = 0;
    int written = 0;

    while (written < len) {
        ret = write(sockfd, &buff[written], len - written);
        if (ret == -1)
            handle_error("failed to write");
        written += ret;
    }

    return 0;
}

int read_stream(int sockfd, char buff[], size_t len) {
    int ret = 0;
    int gotten = 0;

    while (gotten < len) {
        ret = read(sockfd, &buff[gotten], len-gotten);
        if (ret == -1)
            handle_error("failed to read");
        gotten += ret;
    }

    return 0;
}

// socket -> bind -> listen -> accept -> send/recv -> clos
int sock_server()
{
    unlink(SOCKET_UNIX_PATH);
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_error("failed to socket");

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCKET_UNIX_PATH
    };
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)))
        handle_error("failed to bind");

    if (listen(sockfd, LISTEN_BACKLOG)) {
        perror("failed to listen");
        return -1;
    }

    int welcomefd = 0;
    while (1) {
        // NULLs for addr and addrlen for AF_UNIX protocol family
        puts("listening...");
        welcomefd = accept(sockfd, NULL, NULL);
        if (welcomefd == -1)
          handle_error("failed to accept");

        if (fork() != 0) continue;

        char wrchar = 'a';
        while (1) {
            sleep(1);
            if (write_stream(welcomefd, &wrchar, 1) == -1) {
                close(welcomefd);
                perror("failed to write");
            }
            printf("data sent (%c)\n", wrchar);
            wrchar = wrchar+1 > 'z' ? 'a' : wrchar+1;
        }
    }
    
    close(welcomefd);
    return 0;
}

// socket -> connect -> send/recv -> close
int sock_client()
{
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_error("failed to socket");

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCKET_UNIX_PATH
    };
    if (connect(sockfd, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)))
        handle_error("failed to connect");

    char rdbuf[8] = {0};
    while (1) {
        if(read_stream(sockfd, rdbuf, sizeof(rdbuf)) == -1) {
            close(sockfd);
            handle_error("failed to read");
        }
        printf("data recv (%s)\n", rdbuf);
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
