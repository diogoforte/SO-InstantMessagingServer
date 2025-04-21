#ifndef SOCKET_H
#define SOCKET_H

#include <stdbool.h>
#include <netinet/in.h>

typedef struct {
    int (*create_and_bind)(int port);

    bool (*configure)(int socket_fd);

    bool (*listen)(int socket_fd, int backlog);

    void (*close)(int socket_fd);
} SocketUtils;

extern const SocketUtils Socket;

#endif
