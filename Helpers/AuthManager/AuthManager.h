#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H
#include <cstring>

class AuthManager {
private:
    const char* secretKey;
public:
    AuthManager();
    ~AuthManager();
    bool verifyToken(const char* token);
    const char* generateToken(const char* username);
    const char* getSecretKey();
};

#endif // AUTH_MANAGER_H 