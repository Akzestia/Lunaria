#include "QuicServer.h"

int main(){

	QuicServer *server = new QuicServer("10.10.3.201", 6121, 8, "neuxs", "./certs/server.cert", "./certs/server.key");

    server->Start();

    server->Close();

	free(server);
    return 0;
}
