#include "../QuicClient.h"

int main(){

    //Use .beign() to get the first element of the set(Works for single vpn connection only)
    std::string host = VpnManager::getInterfaces().begin()->c_str();
    printf("Host: %s\n", host.c_str());
    QuicClient client(host.c_str(), 6121, "nexus", "certs/server.cert", "certs/server.key");//4433

    client.Connect();

    getchar();
    // client.openPeer("localhost", 6122);
    // getchar();

    client.Disconnect();

    return 0;
}