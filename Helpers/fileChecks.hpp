#ifndef FILE_EXISTS_DEFINED
#define FILE_EXISTS_DEFINED

#include <sys/stat.h> // Include the necessary header file

inline bool fileExists(const char *file) {
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}

#endif // FILE_EXISTS_DEFINED