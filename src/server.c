#include "../lib/inc/libdiogo.h"
#include "../inc/server.h"
#include "../inc/client.h"
#include "../inc/socket.h"

Server *Server_create() {
    Server *server = malloc(sizeof(Server));
    if (!server) return NULL;
    server->name = SERVER_NAME;
    server->clients_head = NULL;
    server->client_count = 0;
    server->running = false;
    server->log_file = NULL;
    pthread_mutex_init(&server->clients_mutex, NULL);
    server->init = Server_init;
    server->start = Server_start;
    server->stop = Server_stop;
    server->add_client = Server_addClient;
    server->cleanup = Server_cleanup;
    return server;
}

void Server_destroy(Server *server) {
    if (server) {
        server->cleanup(server);
        free(server);
    }
}

bool Server_init(Server *self) {
    self->log_file = fopen(LOG_FILE, "a+");
    if (!self->log_file) {
        perror("Failed to open log file");
        return false;
    }
    self->client_socket = Socket.create_and_bind(CLIENT_PORT);
    if (self->client_socket < 0) {
        fclose(self->log_file);
        return false;
    }
    self->admin_socket = Socket.create_and_bind(ADMIN_PORT);
    if (self->admin_socket < 0) {
        Socket.close(self->client_socket);
        fclose(self->log_file);
        return false;
    }
    if (!Socket.listen(self->client_socket, 10) || !Socket.listen(self->admin_socket, 5)) {
        Socket.close(self->client_socket);
        Socket.close(self->admin_socket);
        fclose(self->log_file);
        return false;
    }
    self->running = true;
    return true;
}

static void *handle_client(void *arg) {
    Client *client = arg;
    char buffer[MAX_MSG_LEN];
    d_bzero(buffer, MAX_MSG_LEN);
    client->send_message(client, client->nickname);
    client->send_message(client, " welcome to ");
    client->send_message(client, client->server->name);
    client->send_message(client, "\n");
    while (client->status == STATUS_ACTIVE) {
        const int bytes_received = recv(client->socket, buffer, MAX_MSG_LEN - 1, 0);
        if (bytes_received <= 0)
            break;
        buffer[bytes_received] = '\0';
        time_t now;
        time(&now);
        const struct tm *local_time = localtime(&now);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y%m%d - %H:%M:%S\n", local_time);
        fprintf(client->server->log_file, "%s- %s: %s",
                time_str, client->nickname, buffer);
        d_dprintf(1, "%s- %s: %s\n", time_str, client->nickname, buffer);
    }
    return NULL;
}

static void *accept_client(void *arg) {
    Server *server = arg;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    const int client_sock = accept(server->client_socket, (struct sockaddr *) &client_addr, &addr_len);
    pthread_t accept_thread;
    if (server->client_count < MAX_CLIENTS && pthread_create(&accept_thread, NULL, accept_client, server) != 0) {
        perror("Failed to create accept thread");
        server->running = false;
        return NULL;
    }
    if (client_sock < 0) {
        if (server->running)
            perror("Accept failed");
        return NULL;
    }
    if (send(client_sock, "Please enter your username:\n", 28, 0) < 0) {
        close(client_sock);
        return NULL;
    }
    char nickname[MAX_NICK_LEN];
    const int bytes_received = recv(client_sock, nickname, MAX_NICK_LEN - 1, 0);
    if (bytes_received <= 0) {
        close(client_sock);
        return NULL;
    }
    nickname[bytes_received] = '\0';
    Client *new_client = Client_create(client_sock, nickname, false, server);
    if (!new_client) {
        close(client_sock);
        return NULL;
    }
    if (!server->add_client(server, new_client)) {
        Client_destroy(new_client);
        return NULL;
    }
    printf("New client connected: %s\n", nickname);
    pthread_t client_thread;
    if (pthread_create(&client_thread, NULL, handle_client, new_client) != 0) {
        perror("Failed to create client thread");
        Client_destroy(new_client);
        return NULL;
    }
    new_client->thread_id = client_thread;
    return NULL;
}

void Server_start(Server *self) {
    printf("Starting the server %s on the ports %d (clients) %d (admin)\n", SERVER_NAME, CLIENT_PORT,
           ADMIN_PORT);
    pthread_t accept_thread;
    if (pthread_create(&accept_thread, NULL, accept_client, self) != 0) {
        perror("Failed to create accept thread");
        self->running = false;
        return;
    }
    while (self->running) {
        sleep(5);
        break;
    }
    pthread_join(accept_thread, NULL);
}

void Server_stop(Server *self) {
    self->running = false;
    close(self->client_socket);
    close(self->admin_socket);
}

bool Server_addClient(Server *self, Client *client) {
    ClientNode *new_node = malloc(sizeof(ClientNode));
    if (!new_node) return false;
    new_node->client = client;
    pthread_mutex_lock(&self->clients_mutex);
    new_node->next = self->clients_head;
    self->clients_head = new_node;
    self->client_count++;
    pthread_mutex_unlock(&self->clients_mutex);

    return true;
}

void Server_cleanup(Server *self) {
    if (self->running)
        self->stop(self);

    pthread_mutex_lock(&self->clients_mutex);
    ClientNode *current = self->clients_head;
    while (current != NULL) {
        ClientNode *temp = current;
        current = current->next;
        Client_destroy(temp->client);
        free(temp);
    }
    self->clients_head = NULL;
    self->client_count = 0;
    pthread_mutex_unlock(&self->clients_mutex);

    if (self->log_file) {
        fclose(self->log_file);
        self->log_file = NULL;
    }
    pthread_mutex_destroy(&self->clients_mutex);
}
