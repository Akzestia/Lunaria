#include "QuicClient.h"

int main() {

  QuicClient client = QuicClient("10.10.3.201", 6121, "./certs/server.cert",
                                 "./certs/server.key");
  client.Connect();

  char ch = getchar();
  Person p = Person();
  p.set_name("Azure");
  const size_t dataSize = 20 * 1024 * 1024; // 20MB
  std::string largeData(dataSize, 'A');
  p.set_content(largeData);
  p.set_id(1);
  p.set_email("azure@gmail.com");

  absl::Cord output;

  bool x = p.SerializePartialToCord(&output);

  if (x) {
    client.send(output);
  }

  ch = getchar();

  client.Disconnect();
  return 0;
}
