#include "AuthManager.h"

AuthManager::AuthManager() {
}

AuthManager::~AuthManager() {
}

bool AuthManager::verifyToken(const char* token) {
    return strcmp(token, this->secretKey) == 0;
}

const char* AuthManager::generateToken(const char* username) {
    return this->secretKey;
}

const char* AuthManager::getSecretKey() {
    return this->secretKey;
}