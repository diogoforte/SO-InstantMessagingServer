#include "../inc/socket.h"
#include "../lib/inc/libdiogo.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static int socket_create_and_bind(const int port) {
    const int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Failed to create socket");
        return -1;
    }
    if (!Socket.configure(sock_fd)) {
        close(sock_fd);
        return -1;
    }
    struct sockaddr_in addr;
    d_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Socket binding failed");
        close(sock_fd);
        return -1;
    }
    return sock_fd;
}

static bool socket_configure(const int socket_fd) {
    const int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Socket setsockopt failed");
        return false;
    }
    return true;
}

static bool socket_listen(const int socket_fd, const int backlog) {
    if (listen(socket_fd, backlog) < 0) {
        perror("Socket listen failed");
        return false;
    }
    return true;
}

static void socket_close(const int socket_fd) {
    if (socket_fd >= 0)
        close(socket_fd);
}

const SocketUtils Socket = {
    .create_and_bind = socket_create_and_bind,
    .configure = socket_configure,
    .listen = socket_listen,
    .close = socket_close,
};
