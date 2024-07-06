#include "RequestManager.h"

Lxcode RequestManager::StartSignUpRequest(const Auth &auth) {
    Lxcode code = {true, 0x00};

    if (!auth.has_sign_up())
        return {false, 0x81};

    User u;
    u.set_user_name(auth.sign_up().user_name());
    u.set_user_display_name(auth.sign_up().display_name());
    u.set_user_email(auth.sign_up().user_email());
    u.set_user_password(auth.sign_up().user_password());

    if (!DbManager::addUser(u)) {
        code.is_successful = false;
        code.error_code = 0x82;
    }

    printf("\nUser was signed up successfully");

    return code;
};

RequestManager::~RequestManager() {}