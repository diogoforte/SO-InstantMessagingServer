#include "../inc/libdiogo.h"
#include "server.h"

int main() {
    Server *server = Server_create();
    if (server->init(server)) {
        server->start(server);
    }
    else
        d_dprintf(2, "Failed to initialize server\n");
    Server_destroy(server);
}
