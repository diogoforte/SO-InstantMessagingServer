#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>

typedef struct Client Client;

#define SERVER_NAME "RP's da UAL"
#define LOG_FILE "ims.log"
#define CLIENT_PORT 6500
#define ADMIN_PORT 6600
#define MAX_CLIENTS 100
#define MAX_GROUPS 20
#define MAX_MSG_LEN 1024

typedef struct Server Server;

typedef bool (*ServerInitFunc)(Server *self);

typedef void (*ServerStartFunc)(Server *self);

typedef void (*ServerStopFunc)(Server *self);

typedef bool (*ServerAddClientFunc)(Server *self, Client *client);

typedef void (*ServerRemoveClientFunc)(Server *self, Client *client);

typedef void (*ServerCleanupFunc)(Server *self);

typedef struct ClientNode {
    Client *client;
    struct ClientNode *next;
} ClientNode;

typedef struct {
    Server *server;
    int sock;
    bool is_admin;
} ConnectionInfo;

struct Server {
    char *name;
    ClientNode *clients_head;
    int client_count;
    pthread_mutex_t clients_mutex;
    int client_socket;
    int admin_socket;
    bool has_admin;
    bool running;
    FILE *log_file;

    ServerInitFunc init;
    ServerStartFunc start;
    ServerStopFunc stop;
    ServerAddClientFunc add_client;
    ServerRemoveClientFunc remove_client;
    ServerCleanupFunc cleanup;
};

Server *Server_create();

void Server_destroy(Server *server);

bool Server_init(Server *self);

void Server_start(Server *self);

void Server_stop(Server *self);

bool Server_addClient(Server *self, Client *client);

void Server_removeClient(Server *self, Client *client);

void Server_cleanup(Server *self);

#endif
