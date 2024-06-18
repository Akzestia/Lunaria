#include "../client/QuicClient.h"

int main() {
    QuicClient *client = new QuicClient(
        "10.10.3.201", 6121, "../certs/server.cert", "../certs/server.key");

    client->Connect();

    Person p;

    p.set_name("Akzestia");

    absl::Cord* output;

    if(p.SerializePartialToCord (&(*output)))
        client->send((*output));
    else
        return -1;

    getchar();
    client->Disconnect();
    free(output);
    free(client);
    return 0;

    //test git action x1
}