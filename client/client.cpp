#include "../Entities/Message.h"
#include "../proto/test.pb.h"
#include "QuicClient.h"
#include <cstdio>

int main() {

  QuicClient client = QuicClient("10.10.3.201", 6121, "./certs/server.cert",
                                 "./certs/server.key");
  client.Connect();

  char ch = getchar();

  Message m = Message();

  std::cout << "Hello";
  Person p = Person();
  p.set_name("Aksestia");
  std::cout << "\n" << p.name() << "\n";
  client.send<Message>(m, 0x01);

  ch = getchar();

  client.Disconnect();
  return 0;
}
