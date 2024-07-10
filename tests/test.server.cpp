#include "../server/QuicServer.h"

int main() {
    QuicServer *server =
        new QuicServer("10.10.3.201", 6121, "nexus", "/home/azure/LunariaClient/certs/server.cert",
                       "/home/azure/LunariaClient/certs/server.key");

    server->Start();

    while (server->getIsRunning()) {
        printf("\nPress 'q' to exit\n");
        char exit = getchar();
        if (exit == 'q')
            server->Close();
    }

    delete server;
    return 0;
}