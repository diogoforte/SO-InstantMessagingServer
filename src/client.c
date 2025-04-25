#include "../inc/client.h"
#include "../inc/socket.h"
#include "../lib/inc/libdiogo.h"
#include "../inc/server.h"

Client *Client_create(const int socket, const char *nickname, const bool admin, Server *server) {
    Client *client = malloc(sizeof(Client));
    if (!client) return NULL;
    client->socket = socket;
    char *tmp = malloc(d_strlen(nickname) + 1);
    if (!tmp) {
        free(client);
        return NULL;
    }
    d_strcpy(tmp, nickname);
    client->nickname = d_strtrim(tmp, "\r\n\t");
    if (client->nickname != tmp)
        free(tmp);
    pthread_mutex_lock(&server->clients_mutex);
    const ClientNode *current = server->clients_head;
    while (current) {
        if (!d_strcmp(client->nickname, current->client->nickname)) {
            pthread_mutex_unlock(&server->clients_mutex);
            send(socket, "Error: Duplicate nickname", 26, 0);
            free(client->nickname);
            free(client);
            return NULL;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&server->clients_mutex);
    client->messages_sent = 0;
    client->admin = admin;
    client->status = STATUS_ACTIVE;
    client->server = server;
    client->disconnect = Client_disconnect;
    client->send_message = Client_sendMessage;
    client->server->client_count++;
    printf("New %s connected: \"%s\"\n", admin ? "admin" : "client", client->nickname);
    return client;
}

void Client_destroy(Client *client) {
    if (client) {
        client->disconnect(client);
        free(client);
    }
}

void Client_disconnect(Client *self) {
    if (self->socket != -1) {
        Socket.close(self->socket);
        self->socket = -1;
        if (self->thread_id != 0)
            pthread_join(self->thread_id, NULL);
        printf("The Client %s disconnected from server\n", self->nickname);
    }
    if (self->admin)
        self->server->has_admin = false;
    if (self->nickname)
        free(self->nickname);
    self->server->client_count--;
}

void Client_sendMessage(const Client *self, const char *message) {
    if (self->socket == -1 || self->status != STATUS_ACTIVE) {
        fprintf(stderr, "Not connected to server\n");
        return;
    }
    if (send(self->socket, message, d_strlen(message), 0) == -1)
        perror("Failed to send message");
    else
        printf("Message sent: %s", message);
}
