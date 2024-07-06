#include "EncryptionManager.h"

bool EncryptionManager::ToSHA256(const std::string &input,
                                 std::string &output) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    SHA256_Update(&sha256, input.c_str(), input.size());

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    char hexString[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(hexString + (i * 2), "%02x", hash[i]);
    }
    hexString[SHA256_DIGEST_LENGTH * 2] = 0;

    output = std::string(hexString);

    return true;
}

bool EncryptionManager::encryptMessage(const char *message, const char *key,
                                       char **output) {
    if (message == nullptr || key == nullptr || output == nullptr)
        return false;

    size_t messageLength = strlen(message);
    size_t keyLength = strlen(key);

    if (keyLength == 0)
        return false;
    *output = (char *)malloc(messageLength + 1);
    if (*output == nullptr)
        return false;

    for (size_t i = 0; i < messageLength; ++i)
        (*output)[i] = message[i] ^ key[i % keyLength];
    (*output)[messageLength] = '\0';
    return true;
}

bool EncryptionManager::decryptMessage(const char *encryptedMessage,
                                       const char *key, char **output) {
    if (encryptedMessage == nullptr || key == nullptr || output == nullptr)
        return false;

    size_t messageLength = strlen(encryptedMessage);
    size_t keyLength = strlen(key);

    if (keyLength == 0)
        return false;
    *output = (char *)malloc(messageLength + 1);
    if (*output == nullptr)
        return false;

    for (size_t i = 0; i < messageLength; ++i)
        (*output)[i] = encryptedMessage[i] ^ key[i % keyLength];
    (*output)[messageLength] = '\0';
    return true;
}

bool EncryptionManager::generateKey(const char *userName, char **output) {
    if (userName == nullptr || output == nullptr)
        return false;

    size_t userNameLength = strlen(userName);

    unsigned int hash = 0;
    for (size_t i = 0; i < userNameLength; ++i) {
        hash = (hash * 31) + userName[i];
    }

    char hashString[16];
    snprintf(hashString, sizeof(hashString), "%u", hash);

    *output = (char *)malloc(strlen(hashString) + 1);
    if (*output == nullptr)
        return false;

    strcpy(*output, hashString);
    return true;
}
