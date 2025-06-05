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
    server->log_file = NULL;
    pthread_mutex_init(&server->clients_mutex, NULL);
    server->has_admin = false;
    server->running = false;
    server->total_messages = 0;
    server->groups_head = NULL;
    server->group_count = 0;
    pthread_mutex_init(&server->groups_mutex, NULL);
    server->init = Server_init;
    server->start = Server_start;
    server->stop = Server_stop;
    server->add_client = Server_addClient;
    server->remove_client = Server_removeClient;
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
        self->log_file = NULL;
        return false;
    }
    self->admin_socket = Socket.create_and_bind(ADMIN_PORT);
    if (self->admin_socket < 0) {
        Socket.close(self->client_socket);
        fclose(self->log_file);
        self->log_file = NULL;
        return false;
    }
    if (!Socket.listen(self->client_socket, 10) || !Socket.listen(self->admin_socket, 5)) {
        Socket.close(self->client_socket);
        Socket.close(self->admin_socket);
        fclose(self->log_file);
        self->log_file = NULL;
        return false;
    }
    return self->running = true;
}

static void *handle_client(void *arg) {
    Client *client = arg;
    char buffer[MAX_MSG_LEN];
    d_bzero(buffer, MAX_MSG_LEN);
    char *welcome_message = d_strjoin_var(client->nickname, ", welcome to ", SERVER_NAME, "\n", NULL);
    client->send_message(client, welcome_message);
    free(welcome_message);
    while (client->server->running) {
        const ssize_t bytes_received = recv(client->socket, buffer, MAX_MSG_LEN - 1, 0);
        if (bytes_received <= 0)
            break;
        buffer[bytes_received] = '\0';
        const bool command_handled = Command.process(client, buffer);
        if (client->socket == -1)
            break;
        if (!command_handled)
            client->send_message(client, "Unknown command\n");
    }
    return NULL;
}

static void *handle_username_input(void *arg) {
    ConnectionInfo *info = arg;
    if (send(info->sock, "Please enter your username:\n", 28, 0) < 0) {
        close(info->sock);
        free(info);
        return NULL;
    }
    char nickname[MAX_NICK_LEN];
    const int bytes_received = recv(info->sock, nickname, MAX_NICK_LEN - 1, 0);
    if (bytes_received <= 0) {
        close(info->sock);
        free(info);
        return NULL;
    }
    nickname[bytes_received] = '\0';
    Client *new_client = Client_create(info->sock, nickname, info->is_admin, info->server);
    if (!new_client) {
        close(info->sock);
        free(info);
        return NULL;
    }
    if (!info->server->add_client(info->server, new_client)) {
        Client_destroy(new_client);
        free(info);
        return NULL;
    }
    pthread_t client_thread;
    if (pthread_create(&client_thread, NULL, handle_client, new_client) != 0) {
        perror("Failed to create client thread");
        Client_destroy(new_client);
        free(info);
        return NULL;
    }
    new_client->thread_id = client_thread;
    if (!new_client->server->running)
        pthread_detach(client_thread);
    free(info);
    return NULL;
}

static void handle_new_connection(Server *server, const int sock, const bool is_admin) {
    ConnectionInfo *info = malloc(sizeof(ConnectionInfo));
    if (!info) {
        close(sock);
        return;
    }
    info->server = server;
    info->sock = sock;
    info->is_admin = is_admin;
    pthread_t username_thread;
    if (pthread_create(&username_thread, NULL, handle_username_input, info) != 0) {
        perror("Failed to create username input thread");
        close(sock);
        free(info);
        return;
    }
    pthread_detach(username_thread);
}

static void *accept_client(void *arg) {
    Server *server = arg;
    fd_set read_fds;
    while (server->running) {
        FD_ZERO(&read_fds);
        FD_SET(server->client_socket, &read_fds);
        FD_SET(server->admin_socket, &read_fds);
        const int max_fd = server->client_socket > server->admin_socket ? server->client_socket : server->admin_socket;
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        const int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        if (activity < 0) {
            perror("Select error");
            break;
        }
        if (FD_ISSET(server->client_socket, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            const int client_sock = accept(server->client_socket,
                                           (struct sockaddr *) &client_addr, &addr_len);
            if (client_sock < 0) {
                perror("Accept failed");
                continue;
            }
            if (server->client_count >= MAX_CLIENTS) {
                send(client_sock, "101", 15, 0);
                close(client_sock);
                continue;
            }
            handle_new_connection(server, client_sock, false);
        }
        if (FD_ISSET(server->admin_socket, &read_fds)) {
            struct sockaddr_in admin_addr;
            socklen_t addr_len = sizeof(admin_addr);
            if (server->has_admin) {
                const int temp_sock = accept(server->admin_socket,
                                             (struct sockaddr *) &admin_addr, &addr_len);
                if (temp_sock >= 0) {
                    send(temp_sock, "Only one admin can connect at a time.\n", 37, 0);
                    close(temp_sock);
                }
                continue;
            }
            const int admin_sock = accept(server->admin_socket,
                                          (struct sockaddr *) &admin_addr, &addr_len);
            if (admin_sock < 0) {
                perror("Admin accept failed");
                continue;
            }
            server->has_admin = true;
            handle_new_connection(server, admin_sock, true);
        }
    }
    return NULL;
}

void Server_start(Server *self) {
    printf("Starting the server %s on the ports %d (clients) %d (admin)\n",
           SERVER_NAME, CLIENT_PORT, ADMIN_PORT);
    pthread_t accept_thread;
    if (pthread_create(&accept_thread, NULL, accept_client, self) != 0) {
        perror("Failed to create accept thread");
        self->running = false;
        return;
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
    pthread_mutex_unlock(&self->clients_mutex);
    return true;
}

void Server_removeClient(Server *self, Client *client) {
    pthread_mutex_lock(&self->groups_mutex);
    Group *group = self->groups_head;
    Group *prev_group = NULL;
    while (group) {
        if (group->admin == client) {
            Group *to_delete;
            if (prev_group) {
                prev_group->next = group->next;
                to_delete = group;
                group = group->next;
            } else {
                self->groups_head = group->next;
                to_delete = group;
                group = self->groups_head;
            }
            GroupMember *member = to_delete->members;
            while (member) {
                GroupMember *temp = member;
                member = member->next;
                free(temp);
            }
            free(to_delete->name);
            free(to_delete);
            self->group_count--;
            continue;
        }
        GroupMember *member = group->members;
        GroupMember *prev_member = NULL;
        while (member) {
            if (member->client == client) {
                if (prev_member) {
                    prev_member->next = member->next;
                    free(member);
                } else {
                    group->members = member->next;
                    free(member);
                }
                break;
            }
            prev_member = member;
            member = member->next;
        }
        prev_group = group;
        group = group->next;
    }
    pthread_mutex_unlock(&self->groups_mutex);
    pthread_mutex_lock(&self->clients_mutex);
    if (self->clients_head && !d_strcmp(self->clients_head->client->nickname, client->nickname)) {
        ClientNode *temp = self->clients_head;
        self->clients_head = self->clients_head->next;
        Client_destroy(temp->client);
        free(temp);
        self->client_count--;
    } else {
        ClientNode *current = self->clients_head;
        while (current && current->next) {
            if (!d_strcmp(current->next->client->nickname, client->nickname)) {
                ClientNode *temp = current->next;
                current->next = temp->next;
                Client_destroy(temp->client);
                free(temp);
                self->client_count--;
                break;
            }
            current = current->next;
        }
    }
    pthread_mutex_unlock(&self->clients_mutex);
}

void Server_cleanup(Server *self) {
    if (self->running)
        self->stop(self);
    pthread_mutex_lock(&self->clients_mutex);
    ClientNode *current = self->clients_head;
    while (current) {
        ClientNode *temp = current;
        current = current->next;
        Client_destroy(temp->client);
        free(temp);
    }
    self->clients_head = NULL;
    self->client_count = 0;
    pthread_mutex_unlock(&self->clients_mutex);
    pthread_mutex_lock(&self->groups_mutex);
    Group *group = self->groups_head;
    while (group) {
        Group *to_delete = group;
        group = group->next;
        GroupMember *member = to_delete->members;
        while (member) {
            GroupMember *temp = member;
            member = member->next;
            free(temp);
        }
        free(to_delete->name);
        free(to_delete);
    }
    self->groups_head = NULL;
    self->group_count = 0;
    pthread_mutex_unlock(&self->groups_mutex);
    if (self->log_file) {
        fclose(self->log_file);
        self->log_file = NULL;
    }
    pthread_mutex_destroy(&self->clients_mutex);
    pthread_mutex_destroy(&self->groups_mutex);
}
