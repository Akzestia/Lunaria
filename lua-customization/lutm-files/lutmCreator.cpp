#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>

namespace fs = std::filesystem;

struct LutMFileHeader {
    char magic[4] = {'L', 'U', 'T', 'M'};
    uint32_t version = 1;
    uint64_t content_size;
    uint64_t signature_size;
};

struct FileEntry {
    std::string path;
    std::vector<unsigned char> content;
    bool isDirectory;
};

void serializeDirectory(const fs::path& dir, std::vector<FileEntry>& entries) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        FileEntry fe;
        fe.path = fs::relative(entry.path(), dir).string();
        fe.isDirectory = fs::is_directory(entry);
        if (!fe.isDirectory) {
            std::ifstream ifs(entry.path(), std::ios::binary);
            fe.content = std::vector<unsigned char>(std::istreambuf_iterator<char>(ifs), {});
        }
        entries.push_back(fe);
    }
}

void writeLutmFile(const std::string& outputFile, const std::vector<FileEntry>& entries) {
    LutMFileHeader header;
    std::ofstream ofs(outputFile, std::ios::binary);

    ofs.write(reinterpret_cast<char*>(&header), sizeof(header));

    for (const auto& entry : entries) {
        uint64_t pathSize = entry.path.size();
        uint64_t contentSize = entry.content.size();
        char isDirectory = entry.isDirectory ? 1 : 0;

        ofs.write(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
        ofs.write(entry.path.c_str(), pathSize);
        ofs.write(&isDirectory, sizeof(isDirectory));

        if (!entry.isDirectory) {
            ofs.write(reinterpret_cast<char*>(&contentSize), sizeof(contentSize));
            ofs.write(const_cast<char*>(reinterpret_cast<const char*>(entry.content.data())), contentSize);
        }
    }

    header.content_size = ofs.tellp() - static_cast<std::streamoff>(sizeof(header));
    ofs.seekp(0, std::ios::beg);
    ofs.write(reinterpret_cast<char*>(&header), sizeof(header));
}

void readLutmFile(const std::string& inputFile) {
    std::ifstream ifs(inputFile, std::ios::binary);
    LutMFileHeader header;

    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (strncmp(header.magic, "LUTM", 4) != 0) {
        std::cerr << "Invalid file format." << std::endl;
        return;
    }

    while (ifs) {
        uint64_t pathSize;
        uint64_t contentSize;
        char isDirectory;

        ifs.read(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
        if (!ifs) break;

        std::string path(pathSize, '\0');
        ifs.read(&path[0], pathSize);

        ifs.read(&isDirectory, sizeof(isDirectory));

        if (isDirectory) {
            fs::create_directories("extracted_" + path);
        } else {
            ifs.read(reinterpret_cast<char*>(&contentSize), sizeof(contentSize));
            std::vector<unsigned char> content(contentSize);
            ifs.read(reinterpret_cast<char*>(content.data()), contentSize);

            std::ofstream ofs("extracted_" + path, std::ios::binary);
            ofs.write(reinterpret_cast<char*>(content.data()), content.size());
        }
    }
}

int main() {
    std::vector<FileEntry> entries;

    serializeDirectory("theme_dir", entries);
    writeLutmFile("theme.lutm", entries);
    
    readLutmFile("theme.lutm");

    return 0;
}
