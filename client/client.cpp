#include "QuicClient.h"

int main() {

  QuicClient client = QuicClient("10.10.3.201", 6121, "./certs/server.cert",
                                 "./certs/server.key");
  client.Connect();

  while (getchar() != 'e') {
  }
  client.Disconnect();
  return 0;
}
