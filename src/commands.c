#include "../inc/commands.h"
// #include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../lib/inc/libdiogo.h"

static void idle(Client *client, const char *unused) {
    (void) unused;
    client->status = STATUS_IDLE;
    printf("User %s is now idle.\n", client->nickname);
}

static void active(Client *client, const char *unused) {
    (void) unused;
    client->status = STATUS_ACTIVE;
    printf("User %s is now active.\n", client->nickname);
}

static void cmd_exit(Client *client, const char *unused) {
    (void) unused;
    printf("User %s is disconnecting.\n", client->nickname);
    client->server->remove_client(client->server, client);
}

static void list_users(Client *client, const char *unused) {
    (void) unused;
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    char user_list[MAX_MSG_LEN] = "Connected users:\n";
    while (current) {
        d_strcat(user_list, current->client->nickname);
        d_strcat(user_list, "\n");
        current = current->next;
    }
    pthread_mutex_unlock(&client->server->clients_mutex);
    client->send_message(client, user_list);
}

static void create_group(Server *server, const char *message) {
    (void) server;
    printf("Group creation requested: %s\n", message);
    // TODO: Parse message and implement group creation logic
}

static void add_to_group(Server *server, const char *message) {
    (void) server;
    printf("Add to group requested: %s\n", message);
    // TODO: Parse message and implement logic to add users to a group
}

static void delete_group(Server *server, const char *message) {
    (void) server;
    printf("Group deletion requested: %s\n", message);
    // TODO: Parse message and implement logic to delete a group
}

static void broadcast_all(Client *client, const char *message) {
    if (!message || d_strlen(message) == 0) {
        client->send_message(client, "Error: Message cannot be empty\n");
        return;
    }
    char broadcast_msg[MAX_MSG_LEN];
    snprintf(broadcast_msg, MAX_MSG_LEN, "[BROADCAST] %s: %s", client->nickname, message);
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    while (current) {
        if (current->client->status == STATUS_ACTIVE && current->client != client) {
            current->client->send_message(current->client, broadcast_msg);
        }
        current = current->next;
    }
    pthread_mutex_unlock(&client->server->clients_mutex);
    client->send_message(client, "Broadcast message sent\n");
    printf("Broadcast from %s: %s\n", client->nickname, message);
}

static void send_to_user(Client *client, const char *params) {
    if (!params || d_strlen(params) == 0) {
        client->send_message(client, "Error: Missing recipient and message\n");
        return;
    }
    char *space_pos = d_strchr(params, ' ');
    if (!space_pos || space_pos == params) {
        client->send_message(client, "Error: Invalid format. Use: USER <nick> <message>\n");
        return;
    }
    size_t nick_len = space_pos - params;
    char recipient[MAX_NICK_LEN];
    if (nick_len >= MAX_NICK_LEN) {
        client->send_message(client, "Error: Nickname too long\n");
        return;
    }
    d_strncpy(recipient, params, nick_len);
    recipient[nick_len] = '\0';
    const char *message = space_pos + 1;
    if (d_strlen(message) == 0) {
        client->send_message(client, "Error: Message cannot be empty\n");
        return;
    }
    char private_msg[MAX_MSG_LEN];
    snprintf(private_msg, MAX_MSG_LEN, "[PRIVATE] %s: %s", client->nickname, message);
    bool found = false;
    pthread_mutex_lock(&client->server->clients_mutex);
    ClientNode *current = client->server->clients_head;
    while (current) {
        if (d_strcmp(current->client->nickname, recipient) == 0 &&
            current->client->status == STATUS_ACTIVE) {
            current->client->send_message(current->client, private_msg);
            found = true;
            break;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&client->server->clients_mutex);

    if (found) {
        client->send_message(client, "Private message sent\n");
        printf("Private message from %s to %s\n", client->nickname, recipient);
    } else {
        client->send_message(client, "Error: User not found or not active\n");
    }
}

static void send_to_group(Client *client, const char *params) {
    if (!params || d_strlen(params) == 0) {
        client->send_message(client, "Error: Missing group name and message\n");
        return;
    }
    char *space_pos = d_strchr(params, ' ');
    if (!space_pos || space_pos == params) {
        client->send_message(client, "Error: Invalid format. Use: GROUP <group_name> <message>\n");
        return;
    }
    size_t group_name_len = space_pos - params;
    char group_name[MAX_NICK_LEN];
    if (group_name_len >= MAX_NICK_LEN) {
        client->send_message(client, "Error: Group name too long\n");
        return;
    }
    d_strncpy(group_name, params, group_name_len);
    group_name[group_name_len] = '\0';
    const char *message = space_pos + 1;
    if (d_strlen(message) == 0) {
        client->send_message(client, "Error: Message cannot be empty\n");
        return;
    }
    // TODO: Implement group message sending logic once group structures are defined
    client->send_message(client, "Group messaging not yet implemented\n");
    printf("Group message request from %s to group %s\n", client->nickname, group_name);
}

static void list_all_users(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }

    char user_list[MAX_MSG_LEN] = "All users and their status:\n";
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    while (current) {
        char status_str[10];
        if (current->client->status == STATUS_ACTIVE) {
            d_strcpy(status_str, "ACT");
        } else {
            d_strcpy(status_str, "IDL");
        }
        char user_entry[MAX_NICK_LEN + 15];
        snprintf(user_entry, sizeof(user_entry), "%s - %s\n", current->client->nickname, status_str);
        d_strcat(user_list, user_entry);
        current = current->next;
    }
    pthread_mutex_unlock(&client->server->clients_mutex);
    client->send_message(client, user_list);
    printf("Admin %s requested user list\n", client->nickname);
}

static void list_groups(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }
    // TODO: Replace this with actual group listing when groups are implemented
    client->send_message(client, "Group listing not yet implemented\n");
    printf("Admin %s requested group list\n", client->nickname);
}

static void list_message_stats(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }
    // TODO: Replace this placeholder with actual message statistics tracking
    char stats[MAX_MSG_LEN] = "Message Statistics:\n";
    d_strcat(stats, "Total messages: 0\n\n");
    d_strcat(stats, "Per user statistics:\n");
    d_strcat(stats, "(Message tracking not yet implemented)\n");

    client->send_message(client, stats);
    printf("Admin %s requested message statistics\n", client->nickname);
}

static void terminate_server(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }
    printf("Admin %s has requested server termination\n", client->nickname);
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    while (current) {
        if (current->client->status == STATUS_ACTIVE) {
            const char *shutdown_msg = "The server will terminate. Thank you and see you next time.\n";
            current->client->send_message(current->client, shutdown_msg);
        }
        current = current->next;
    }
    pthread_mutex_unlock(&client->server->clients_mutex);
    sleep(1);
    client->server->stop(client->server);
}

void handle_create_group(Client *client, const char *params) {
    create_group(client->server, params);
}

void handle_add_to_group(Client *client, const char *params) {
    add_to_group(client->server, params);
}

void handle_delete_group(Client *client, const char *params) {
    delete_group(client->server, params);
}

static bool process_command(Client *client, const char *buffer) {
    const CommandEntry commands[] = {
        {"IDL", 3, idle},
        {"ACT", 3, active},
        {"EXT", 3, cmd_exit},
        {"LST", 3, list_users},
        {"ALL ", 4, broadcast_all},
        {"USER ", 5, send_to_user},
        {"GROUP ", 6, send_to_group},
        {"GRPC ", 5, handle_create_group},
        {"GRPA ", 5, handle_add_to_group},
        {"GRPD ", 5, handle_delete_group},
        {"LU", 2, list_users},
        {"LG", 2, list_groups},
        {"LNM", 3, list_message_stats}
    };
    const size_t command_count = sizeof(commands) / sizeof(commands[0]);
    if (d_strncmp(buffer, "EXT", 3) == 0 && client->admin) {
        terminate_server(client, NULL);
        return true;
    }
    for (size_t i = 0; i < command_count; i++) {
        if (d_strncmp(buffer, commands[i].prefix, commands[i].prefix_len) == 0) {
            commands[i].handler(client, buffer + commands[i].prefix_len);
            return true;
        }
    }
    return false;
}

const CommandUtils Command = {
    .idle = idle,
    .active = active,
    .exit = cmd_exit,
    .list_users = list_users,
    .create_group = create_group,
    .add_to_group = add_to_group,
    .delete_group = delete_group,
    .broadcast_all = broadcast_all,
    .send_to_user = send_to_user,
    .send_to_group = send_to_group,
    .list_all_users = list_all_users,
    .list_groups = list_groups,
    .list_message_stats = list_message_stats,
    .terminate_server = terminate_server,
    .process = process_command
};
