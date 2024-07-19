#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <string>

class AuthManager {
private:
    const char* secretKey = "my_secret";
public:
    AuthManager();
    ~AuthManager();
    bool verifyToken(const char*, const char*, const char*);
    static const char* generateToken(const char*, const char*);
    static const char* getSecretKey();
};

#endif // AUTH_MANAGER_H 