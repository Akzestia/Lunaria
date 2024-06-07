#include "QuicClient.h"
#include <cstdio>
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
    char cc = getchar();
    // User u;

    // u.set_name("Akzestia");
    // u.set_email("akzestia@xxx.com");
    Person p;
    p.set_email("Akzestia");
    p.set_name("akzestia@xxx.com");

    Wrapper w;

    // *w.mutable_user() = u;
    *w.mutable_person() = p;

    absl::Cord cord;
    bool serialized = w.SerializePartialToCord(&cord);

    if(serialized){
        client.send(cord);
    }

    char ch = getchar();
    
    client.Disconnect();

    char cx = getchar();
    client.Connect();

    char cu = getchar();
    
    client.Disconnect();
    return 0;
}
