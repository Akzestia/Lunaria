#include "../server/QuicServer.h"


int main(){
    QuicServer* server = new QuicServer("10.10.3.201", 6121, "nexus", "../certs/server.cert", "../certs/server.key");

    server->Start();

    server->Close();

	free(server);
    return 0;
}