#include "EncryptionManager.h"

bool EncryptionManager::encryptMessage(const char* message, const char* key, char** output) {
    if (message == nullptr || key == nullptr || output == nullptr)
        return false;

    size_t messageLength = strlen(message);
    size_t keyLength = strlen(key);

    if (keyLength == 0)
        return false;
    *output = (char*)malloc(messageLength + 1);
    if (*output == nullptr)
        return false;

    for (size_t i = 0; i < messageLength; ++i)
        (*output)[i] = message[i] ^ key[i % keyLength];
    (*output)[messageLength] = '\0'; 
    return true;
}

bool EncryptionManager::decryptMessage(const char* encryptedMessage, const char* key, char** output) {
    if (encryptedMessage == nullptr || key == nullptr || output == nullptr)
        return false;

    size_t messageLength = strlen(encryptedMessage);
    size_t keyLength = strlen(key);

    if (keyLength == 0)
        return false;
    *output = (char*)malloc(messageLength + 1);
    if (*output == nullptr)
        return false;

    for (size_t i = 0; i < messageLength; ++i)
        (*output)[i] = encryptedMessage[i] ^ key[i % keyLength];
    (*output)[messageLength] = '\0'; 
    return true;
}

bool EncryptionManager::generateKey (const char* userName, char** output){
if (userName == nullptr || output == nullptr)
        return false;

    size_t userNameLength = strlen(userName);

    unsigned int hash = 0;
    for (size_t i = 0; i < userNameLength; ++i) {
        hash = (hash * 31) + userName[i];
    }

    char hashString[16];
    snprintf(hashString, sizeof(hashString), "%u", hash);

    *output = (char*)malloc(strlen(hashString) + 1);
    if (*output == nullptr)
        return false;

    strcpy(*output, hashString);
    return true;
}
