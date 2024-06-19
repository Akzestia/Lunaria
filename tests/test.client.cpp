#include "../client/QuicClient.h"

int main() {
    QuicClient *client = new QuicClient("10.10.3.201", 6121, "nexus", "../certs/server.cert", "../certs/server.key");

    client->Connect();

    Person* p = new Person();
    p->set_name("Akzestia");

    Wrapper* w = new Wrapper();

    *w->mutable_person() = (*p);

    absl::Cord* output = new absl::Cord();

    if (w->SerializePartialToCord(output)) {
        std::cout << *output << "\n"; 
        client->send(*output);
    } else {
        delete output;
        delete client;
        delete p;
        delete w;
        return -1;
    }

    getchar();
    client->Disconnect();
    delete output;
    delete client;
    delete p;
    delete w;
    return 0;
}

