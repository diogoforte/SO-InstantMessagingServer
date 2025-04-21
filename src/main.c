#include "libdiogo.h"
#include "server.h"

int main() {
    Server *server = Server_create();
    if (server->init(server))
        server->start(server);
    else
        fprintf(stderr, "Failed to initialize server\n");
    Server_destroy(server);
    return 0;
}
