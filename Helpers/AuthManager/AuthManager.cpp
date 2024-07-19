#include "AuthManager.h"

AuthManager::AuthManager() {
}

AuthManager::~AuthManager() {
}

bool AuthManager::verifyToken(const char* username, const char* password, const char* token) {
    const char* generatedToken = generateToken(username, password);
    bool result = strcmp(generatedToken, token) == 0;
    return result;
}

const char* AuthManager::generateToken(const char* username, const char* password) {
    std::string data = std::string(username) + password;

    unsigned char* digest;
    digest = HMAC(EVP_sha256(), secretKey, strlen(secretKey), reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), NULL, NULL);

    char* hexDigest = new char[SHA256_DIGEST_LENGTH*2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(&hexDigest[i*2], "%02x", (unsigned int)digest[i]);
    }

    return hexDigest;
}

const char* AuthManager::getSecretKey() {
    return this->secretKey;
}