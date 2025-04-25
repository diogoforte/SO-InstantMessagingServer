#include "../inc/commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
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
    char **message_split = d_split(message, ' ');
    if (!message_split || !message_split[0] || !message_split[1]) {
        d_freematrix(message_split);
        return;
    }
    pthread_mutex_lock(&server->groups_mutex);
    for (const Group *group = server->groups_head; group; group = group->next) {
        if (!d_strcmp(group->name, message_split[0])) {
            pthread_mutex_unlock(&server->groups_mutex);
            d_freematrix(message_split);
            return;
        }
    }
    Client *admin_client = NULL;
    pthread_mutex_lock(&server->clients_mutex);
    for (const ClientNode *node = server->clients_head; node; node = node->next) {
        if (!d_strcmp(node->client->nickname, message_split[1])) {
            admin_client = node->client;
            break;
        }
    }
    pthread_mutex_unlock(&server->clients_mutex);
    if (!admin_client) {
        pthread_mutex_unlock(&server->groups_mutex);
        d_freematrix(message_split);
        return;
    }
    Group *new_group = malloc(sizeof(Group));
    if (!new_group) {
        pthread_mutex_unlock(&server->groups_mutex);
        d_freematrix(message_split);
        return;
    }
    new_group->name = d_strdup(message_split[0]);
    if (!new_group->name) {
        free(new_group);
        pthread_mutex_unlock(&server->groups_mutex);
        d_freematrix(message_split);
        return;
    }
    new_group->admin = admin_client;
    new_group->members = NULL;
    new_group->next = server->groups_head;
    server->groups_head = new_group;
    server->group_count++;
    pthread_mutex_unlock(&server->groups_mutex);
    Command.add_to_group(server, message);
    d_freematrix(message_split);
}

static void add_to_group(Server *server, const char *message) {
    char **message_split = d_split(message, ' ');
    if (!message_split || !message_split[0] || !message_split[1]) {
        d_freematrix(message_split);
        return;
    }
    Client *admin_client = NULL;
    pthread_mutex_lock(&server->clients_mutex);
    for (const ClientNode *node = server->clients_head; node; node = node->next) {
        if (!d_strcmp(node->client->nickname, message_split[1])) {
            admin_client = node->client;
            break;
        }
    }
    pthread_mutex_unlock(&server->clients_mutex);
    if (!admin_client) {
        d_freematrix(message_split);
        return;
    }
    pthread_mutex_lock(&server->groups_mutex);
    Group *group = NULL;
    for (Group *g = server->groups_head; g; g = g->next) {
        if (!d_strcmp(g->name, message_split[0])) {
            group = g;
            break;
        }
    }
    if (!group || d_strcmp(group->admin->nickname, message_split[1])) {
        pthread_mutex_unlock(&server->groups_mutex);
        d_freematrix(message_split);
        return;
    }
    pthread_mutex_lock(&server->clients_mutex);
    size_t i = 0;
    while (message_split[1 + i]) {
        char *client_nickname = d_strtrim(message_split[1 + i], "\r\n\t ");
        Client *member_client = NULL;
        for (const ClientNode *node = server->clients_head; node; node = node->next) {
            if (!d_strcmp(node->client->nickname, client_nickname)) {
                member_client = node->client;
                break;
            }
        }
        if (member_client) {
            bool already_member = false;
            for (const GroupMember *m = group->members; m; m = m->next) {
                if (m->client == member_client) {
                    already_member = true;
                    break;
                }
            }
            if (!already_member) {
                GroupMember *new_member = malloc(sizeof(GroupMember));
                if (new_member) {
                    new_member->client = member_client;
                    new_member->next = group->members;
                    group->members = new_member;
                }
            }
        }
        free(client_nickname);
        i++;
    }
    pthread_mutex_unlock(&server->clients_mutex);
    pthread_mutex_unlock(&server->groups_mutex);
    d_freematrix(message_split);
}

static void delete_group(Server *server, const char *message) {
    char **message_split = d_split(message, ' ');
    if (!message_split || !message_split[0] || !message_split[1]) {
        d_freematrix(message_split);
        return;
    }
    Client *admin_client = NULL;
    char *admin_nickname = d_strtrim(message_split[1], "\r\n\t ");
    pthread_mutex_lock(&server->clients_mutex);
    for (const ClientNode *node = server->clients_head; node; node = node->next) {
        if (!d_strcmp(node->client->nickname, admin_nickname)) {
            admin_client = node->client;
            break;
        }
    }
    pthread_mutex_unlock(&server->clients_mutex);
    if (!admin_client) {
        d_freematrix(message_split);
        return;
    }
    pthread_mutex_lock(&server->groups_mutex);
    Group *prev = NULL;
    Group *curr = server->groups_head;
    while (curr) {
        if (!d_strcmp(curr->name, message_split[0])) {
            if (!d_strcmp(curr->admin->nickname, admin_nickname)) {
                if (!prev)
                    server->groups_head = curr->next;
                else
                    prev->next = curr->next;
                GroupMember *member = curr->members;
                while (member) {
                    GroupMember *temp = member;
                    member = member->next;
                    free(temp);
                }
                free(curr->name);
                free(curr);
                server->group_count--;
                break;
            }
        }
        prev = curr;
        curr = curr->next;
    }
    pthread_mutex_unlock(&server->groups_mutex);
    d_freematrix(message_split);
    free(admin_nickname);
}

static void broadcast_all(Client *client, const char *message) {
    if (!message || d_strlen(message) == 0) {
        client->send_message(client, "Error: Message cannot be empty\n");
        return;
    }
    char broadcast_msg[MAX_MSG_LEN];
    char time_str[20];
    get_time_str(time_str, sizeof(time_str));
    pthread_mutex_lock(&client->server->clients_mutex);
    fprintf(client->server->log_file, "%s - [BROADCAST] %s: %s", time_str, client->nickname, message);
    fflush(client->server->log_file);
    pthread_mutex_unlock(&client->server->clients_mutex);
    snprintf(broadcast_msg, MAX_MSG_LEN, "%s - [BROADCAST] %s: %s", time_str, client->nickname, message);
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
    client->messages_sent++;
    client->server->total_messages++;
}

static void send_to_user(Client *client, const char *params) {
    if (!params || !d_strlen(params)) {
        client->send_message(client, "Error: Missing recipient and message\n");
        return;
    }
    char *space_pos = d_strchr(params, ' ');
    if (!space_pos || space_pos == params) {
        client->send_message(client, "Error: Invalid format. Use: USER <nick> <message>\n");
        return;
    }
    const size_t nick_len = space_pos - params;
    char recipient[MAX_NICK_LEN];
    if (nick_len >= MAX_NICK_LEN) {
        client->send_message(client, "Error: Nickname too long\n");
        return;
    }
    d_strncpy(recipient, params, nick_len);
    recipient[nick_len] = '\0';
    const char *message = space_pos + 1;
    if (!d_strlen(message)) {
        client->send_message(client, "Error: Message cannot be empty\n");
        return;
    }
    if (!d_strcmp(client->nickname, recipient)) {
        client->send_message(client, "Error: You can not message yourself\n");
        return;
    }
    char private_msg[MAX_MSG_LEN];
    char time_str[20];
    get_time_str(time_str, sizeof(time_str));
    pthread_mutex_lock(&client->server->clients_mutex);
    fprintf(client->server->log_file, "%s - [PRIVATE] %s to %s: %s", time_str, client->nickname, recipient, message);
    fflush(client->server->log_file);
    pthread_mutex_unlock(&client->server->clients_mutex);
    snprintf(private_msg, MAX_MSG_LEN, "%s - [PRIVATE] %s: %s", time_str, client->nickname, message);
    bool found = false;
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    while (current) {
        if (!d_strcmp(current->client->nickname, recipient) &&
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
        client->messages_sent++;
        client->server->total_messages++;
    } else
        client->send_message(client, "Error: User not found or not active\n");
}

static void send_to_group(Client *client, const char *params) {
    if (!params || !d_strlen(params)) {
        client->send_message(client, "Error: Missing group name and message\n");
        return;
    }
    char *space_pos = d_strchr(params, ' ');
    if (!space_pos || space_pos == params) {
        client->send_message(client, "Error: Invalid format. Use: GROUP <group_name> <message>\n");
        return;
    }
    const size_t group_name_len = space_pos - params;
    char group_name[MAX_NICK_LEN];
    if (group_name_len >= MAX_NICK_LEN) {
        client->send_message(client, "Error: Group name too long\n");
        return;
    }
    d_strncpy(group_name, params, group_name_len);
    group_name[group_name_len] = '\0';
    const char *message = space_pos + 1;
    if (!d_strlen(message)) {
        client->send_message(client, "Error: Message cannot be empty\n");
        return;
    }
    char group_msg[MAX_MSG_LEN];
    char time_str[20];
    get_time_str(time_str, sizeof(time_str));
    bool found_group = false;
    bool is_member = false;
    pthread_mutex_lock(&client->server->groups_mutex);
    const Group *group = client->server->groups_head;
    while (group) {
        if (!d_strcmp(group->name, group_name)) {
            found_group = true;
            const GroupMember *member = group->members;
            while (member) {
                if (member->client == client) {
                    is_member = true;
                    break;
                }
                member = member->next;
            }
            if (is_member) {
                pthread_mutex_lock(&client->server->clients_mutex);
                fprintf(client->server->log_file, "%s - [GROUP-%s] %s: %s\n",
                        time_str, group->name, client->nickname, message);
                fflush(client->server->log_file);
                pthread_mutex_unlock(&client->server->clients_mutex);
                snprintf(group_msg, MAX_MSG_LEN, "%s - [GROUP-%s] %s: %s\n",
                         time_str, group->name, client->nickname, message);
                const GroupMember *curr_member = group->members;
                while (curr_member) {
                    if (curr_member->client != client && curr_member->client->status == STATUS_ACTIVE)
                        curr_member->client->send_message(curr_member->client, group_msg);
                    curr_member = curr_member->next;
                }
                client->send_message(client, "Group message sent\n");
                client->messages_sent++;
                client->server->total_messages++;
            }
            break;
        }
        group = group->next;
    }
    pthread_mutex_unlock(&client->server->groups_mutex);
    if (!found_group)
        client->send_message(client, "Error: Group does not exist\n");
    else if (!is_member)
        client->send_message(client, "Error: You are not a member of this group\n");
}

static void list_all_users(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }
    char user_list[MAX_MSG_LEN] = "Users and Status:\n";
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    while (current) {
        char status_str[10];
        current->client->status == STATUS_ACTIVE ? d_strcpy(status_str, "ACT") : d_strcpy(status_str, "IDL");
        char user_entry[MAX_NICK_LEN + 15];
        snprintf(user_entry, sizeof(user_entry), "%s - %s\n", current->client->nickname, status_str);
        d_strcat(user_list, user_entry);
        current = current->next;
    }
    pthread_mutex_unlock(&client->server->clients_mutex);
    client->send_message(client, user_list);
}

static void list_groups(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }
    char group_list[MAX_MSG_LEN] = "Registered groups:\n";
    pthread_mutex_lock(&client->server->groups_mutex);
    Group *current = client->server->groups_head;
    if (!current) {
        d_strcat(group_list, "No groups found\n");
    } else {
        while (current) {
            char group_entry[MAX_MSG_LEN];
            snprintf(group_entry, sizeof(group_entry), "- %s (Admin: %s)\n  Members: ",
                     current->name, current->admin->nickname);
            d_strcat(group_list, group_entry);

            const GroupMember *member = current->members;
            bool first = true;
            while (member) {
                if (!first) {
                    d_strcat(group_list, ", ");
                }
                d_strcat(group_list, member->client->nickname);
                first = false;
                member = member->next;
            }
            d_strcat(group_list, "\n");
            current = current->next;
        }
    }
    pthread_mutex_unlock(&client->server->groups_mutex);
    client->send_message(client, group_list);
}

static void list_message_stats(Client *client, const char *unused) {
    (void) unused;
    if (!client->admin) {
        client->send_message(client, "Error: Admin privileges required\n");
        return;
    }
    char stats[MAX_MSG_LEN] = "Message Statistics:\n";
    char total[64];
    snprintf(total, sizeof(total), "Total messages: %d\n\n", client->server->total_messages);
    d_strcat(stats, total);
    d_strcat(stats, "Per user statistics:\n");
    pthread_mutex_lock(&client->server->clients_mutex);
    const ClientNode *current = client->server->clients_head;
    if (!current)
        d_strcat(stats, "No active users\n");
    else {
        while (current) {
            char user_stats[MAX_NICK_LEN + 32];
            snprintf(user_stats, sizeof(user_stats), "- %s: %d messages\n",
                     current->client->nickname, current->client->messages_sent);
            if (d_strlen(stats) + d_strlen(user_stats) < MAX_MSG_LEN - 1) {
                d_strcat(stats, user_stats);
            } else {
                d_strcat(stats, "...(truncated)\n");
                break;
            }
            current = current->next;
        }
    }
    pthread_mutex_unlock(&client->server->clients_mutex);
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
        {"LU", 2, list_all_users},
        {"LG", 2, list_groups},
        {"LNM", 3, list_message_stats}
    };
    const size_t command_count = sizeof(commands) / sizeof(commands[0]);
    if (!d_strncmp(buffer, "EXT", 3) && client->admin) {
        terminate_server(client, NULL);
        return true;
    }
    for (size_t i = 0; i < command_count; i++) {
        if (!d_strncmp(buffer, commands[i].prefix, commands[i].prefix_len)) {
            commands[i].handler(client, buffer + commands[i].prefix_len);
            return true;
        }
    }
    return false;
}

void get_time_str(char *buffer, const size_t size) {
    time_t now;
    time(&now);
    const struct tm *local_time = localtime(&now);
    d_bzero(buffer, size);
    strftime(buffer, size, "%Y%m%d - %H:%M:%S", local_time);
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
