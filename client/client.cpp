#include "../Entities/Message.h"
#include "QuicClient.h"
#include <cstdio>
#include "../proto/test.pb.h"

int main() {

  QuicClient client = QuicClient("10.10.3.201", 6121, "./certs/server.cert",
                                 "./certs/server.key");
  client.Connect();

  char ch = getchar();

  Message m = Message();

  Person p = Person();
  p.set_name("Aksestia");
  std::cout << p.name() << "\n";
  client.send<Message>(m, 0x01);
  
  ch = getchar();
  
  client.Disconnect();
  return 0;
}
