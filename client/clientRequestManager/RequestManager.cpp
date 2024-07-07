#include "RequestManager.h"

Lxcode RequestManager::StartSignUpRequest(const Auth &auth) {
    Lxcode code = {true, SUCCESS};

    if (!auth.has_sign_up())
        return {false, 0x81};

    User u;
    u.set_user_name(auth.sign_up().user_name());
    u.set_display_name(auth.sign_up().display_name());
    u.set_user_email(auth.sign_up().user_email());
    u.set_user_password(auth.sign_up().user_password());

    if (!DbManager::addUser(u)) {
        code.is_successful = false;
        code.error_code = DB_ERROR_FAILED_TO_ADD_USER;
    }

    printf("\nUser was signed up successfully");

    return code;
};

Lxcode RequestManager::StartSignInRequest(const Auth &auth) {
    Lxcode code = {true, 0x00};

    if (!auth.has_sign_up())
        return {false, 0x81};

    User* u = new User();

    if (!DbManager::getUser(auth, u).is_successful) {
        code.is_successful = false;
        code.error_code = DB_ERROR_USER_NOT_FOUND;
    }
    printf("\nUser was signed up successfully");

    auth.sign_up().user_email();

    delete u;

    return code;
};


RequestManager::~RequestManager() {}