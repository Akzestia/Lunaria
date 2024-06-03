#include "QuicClient.h"
#include <fstream>
#include <vector>

std::vector<uint8_t> loadFileContent(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary | std::ios::ate);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> fileData(fileSize);
  if (!file.read(reinterpret_cast<char *>(fileData.data()), fileSize)) {
    throw std::runtime_error("Failed to read file: " + filePath);
  }

  return fileData;
}

int main() {
  QuicClient client("10.10.3.201", 6121, "./certs/server.cert",
                    "./certs/server.key");
  client.Connect();

  Person p;
  p.set_name("Azure");
  p.set_id(1);
  p.set_email("azure@gmail.com");

  std::vector<std::string> filePaths = {
      "/home/azure/Documents/GitHub/Linux-x64-HTTP3/images/Shinka.gif",
      "/home/azure/Documents/GitHub/Linux-x64-HTTP3/images/hCxW040.jpeg",
      "/home/azure/Documents/GitHub/Linux-x64-HTTP3/images/sunaookami-shiroko.mp4",
      "/home/azure/Documents/GitHub/Linux-x64-HTTP3/images/mitsuha.mp4"};

  for (const auto &filePath : filePaths) {
    std::vector<uint8_t> fileData = loadFileContent(filePath);
    p.add_content(std::string(fileData.begin(), fileData.end()));
  }

  absl::Cord output;
  bool serialized = p.SerializePartialToCord(&output);

  if (serialized) {
    client.send(output);
  }

  char ch = getchar();
  client.Disconnect();

  return 0;
}
