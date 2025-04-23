#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"
#include "server.h"

void get_time_str(char *buffer, size_t size);

typedef struct {
    void (*idle)(Client *client, const char *unused);

    void (*active)(Client *client, const char *unused);

    void (*exit)(Client *client, const char *unused);

    void (*list_users)(Client *client, const char *unused);

    void (*create_group)(Server *server, const char *message);

    void (*add_to_group)(Server *server, const char *message);

    void (*delete_group)(Server *server, const char *message);

    void (*broadcast_all)(Client *client, const char *message);

    void (*send_to_user)(Client *client, const char *params);

    void (*send_to_group)(Client *client, const char *params);

    void (*list_all_users)(Client *client, const char *unused);

    void (*list_groups)(Client *client, const char *unused);

    void (*list_message_stats)(Client *client, const char *unused);

    void (*terminate_server)(Client *client, const char *unused);

    bool (*process)(Client *client, const char *buffer);
} CommandUtils;

typedef struct {
    const char *prefix;
    size_t prefix_len;

    void (*handler)(Client *, const char *);
} CommandEntry;

extern const CommandUtils Command;

#endif
