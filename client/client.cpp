#include "QuicClient.h"

int main() {

  QuicClient client = QuicClient("10.10.3.201", 6121, "./certs/server.cert",
                                 "./certs/server.key");
  client.Connect();

  char ch = getchar();

  Message m = Message();
  std::cout << "Hello";
  Person p = Person();
  std::string name(50'000'000, 'A'); // 50MB string of 'A'
  std::cout << "\n\nName Created\n\n";
  p.set_name(name);
  p.set_id(1);
  p.set_email("azure@gmail.com");

  absl::Cord output;

  bool x = p.SerializePartialToCord(&output);

	if(x){
		client.send(output);
	}

  ch = getchar();

  client.Disconnect();
  return 0;
}
