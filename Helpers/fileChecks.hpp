#ifndef FILE_EXISTS_DEFINED
#define FILE_EXISTS_DEFINED

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

inline bool fileExists(const char *file) {
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}

inline std::string getFileExtension(const std::vector<uint8_t> &fileData) {
    if (fileData.empty()) {
        return "";
    }
    size_t index = 0;
    while (index < fileData.size() && fileData[index] == 0x00) {
        ++index;
    }
    std::cout << "First non null: " << index << "\n";

    std::printf("First three bytes: 0x%02X 0x%02X 0x%02X\n", fileData[index],
                fileData[index + 1], fileData[index + 2]);

    if (index + 4 <= fileData.size()) {
        if (fileData[index] == 0xFF && fileData[index + 1] == 0xD8 &&
            fileData[index + 2] == 0xFF) {
            return ".jpg"; // JPEG file
        } else if (fileData[index] == 0x89 && fileData[index + 1] == 0x50 &&
                   fileData[index + 2] == 0x4E && fileData[index + 3] == 0x47) {
            return ".png"; // PNG file
        } else if (fileData[index] == 0x47 && fileData[index + 1] == 0x49 &&
                   fileData[index + 2] == 0x46) {
            return ".gif"; // GIF file
        } else if (fileData[index] == 0x00 && fileData[index + 1] == 0x00 &&
                   fileData[index + 2] == 0x02 && fileData[index + 3] == 0x00) {
            return ".cur"; // CUR file
        } else if (fileData[index] == 0x49 && fileData[index + 1] == 0x44 &&
                   fileData[index + 2] == 0x33 &&
                   (fileData[index + 3] == 0x02 ||
                    fileData[index + 3] == 0x01)) {
            return ".mp3"; // MP3 file
        } else if (fileData[index] == 0x20 && fileData[index + 1] == 0x66 &&
                   fileData[index + 2] == 0x74) {
            return ".mp4"; // MP4 file
        }
    }

    return ""; // Unknown file type
}

#endif // FILE_EXISTS_DEFINED
