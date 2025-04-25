#ifndef CLIENT_H
#define CLIENT_H

#include "commands.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct Server Server;

#define MAX_NICK_LEN 32
#define STATUS_ACTIVE 0
#define STATUS_IDLE 1

typedef struct Client Client;

typedef void (*ClientConnectFunc)(Client *self, const char *server_ip, int port);

typedef void (*ClientDisconnectFunc)(Client *self);

typedef void (*ClientSendMsgFunc)(const Client *self, const char *message);

struct Client {
    int socket;
    char *nickname;
    bool admin;
    int status;
    int messages_sent;
    pthread_t thread_id;
    Server *server;

    ClientConnectFunc connect;
    ClientDisconnectFunc disconnect;
    ClientSendMsgFunc send_message;
};

Client *Client_create(int socket, const char *nickname, bool admin, Server *server);

void Client_destroy(Client *client);

void Client_connect(Client *self, const char *server_ip, int port);

void Client_disconnect(Client *self);

void Client_sendMessage(const Client *self, const char *message);

#endif
