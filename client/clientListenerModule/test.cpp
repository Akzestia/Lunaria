#include "../QuicClient.h"

int main(){
    QuicClient client("localhost", 6121, "nexus", "certs/server.cert", "certs/server.key");//4433

    client.Connect();

    getchar();
    client.openPeer("localhost", 6122);
    getchar();

    client.Disconnect();

    return 0;
}