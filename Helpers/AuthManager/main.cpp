#include "AuthManager.h"
#include <iostream>
int main(){

    AuthManager authManager;

    const char* username = "admin";
    const char* password = "admin";

    const char* token = authManager.generateToken(username, password);

    if (authManager.verifyToken(username, password, token)) {
        std::cout << "Token is valid" << std::endl;
    } else {
        std::cout << "Token is invalid" << std::endl;
    }

    return 0;
}