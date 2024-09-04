#include "RouteManager.h"
#include "../error-manager/ErrorManager.h"
#include <cstdio>
#include <variant>

void RouteManager::InitScyllaDb() { ScyllaManager::initScyllaManager(); }

Lxcode RouteManager::handleReport(const Payload &payload) {
    Lxcode return_code;
    return_code.error_code = 0x00;
    return_code.is_successful = true;
    if (std::holds_alternative<Report>(payload)) {
        const Report &report = std::get<Report>(payload);

        return return_code;
    } else {
        return_code.error_code = 0x01;
        return_code.is_successful = false;
        return return_code;
    }
}

Lxcode RouteManager::handleSignUp(const SignUpRequest &sign_up) {

    Lxcode return_code = Lxcode::OK();

#ifdef USE_SCYLLA_DB
    return_code = ScyllaManager::createUser(sign_up);
#else
    // User u;
    // u.set_user_name(sign_up.user_name());
    // u.set_user_email(sign_up.user_email());
    // u.set_user_password(sign_up.user_password());

    // return_code = DbManager::addUser(u);
#endif

    if (return_code == Lxcode::OK()) {
        return_code.response = AuthManager::generateToken(
            std::get<User *>(return_code.payload)->user_name().c_str(),
            std::get<User *>(return_code.payload)->user_password().c_str());
        return return_code;
    }

    return return_code;
}

Lxcode RouteManager::handleSignIn(const SignInRequest &si) {
    Lxcode return_code = Lxcode::OK();
#ifdef USE_SCYLLA_DB
    return_code = ScyllaManager::getUser(si);
#else
    // return_code = DbManager::getUser(si);
#endif

    if (return_code == Lxcode::OK()) {

        return_code.response = AuthManager::generateToken(
            std::get<User *>(return_code.payload)->user_name().c_str(),
            std::get<User *>(return_code.payload)->user_password().c_str());
        return return_code;
    }

    return return_code;
}

Lxcode RouteManager::getMessages(const Payload &payload, std::set<Message> &) {
    Lxcode return_code;
    return_code.error_code = 0x00;
    return_code.is_successful = true;
    if (std::holds_alternative<User>(payload)) {
        const User &user = std::get<User>(payload);

        return return_code;
    } else {
        return_code.error_code = 0x01;
        return_code.is_successful = false;
        return return_code;
    }
}

Lxcode RouteManager::createContact(const Payload &payload) {

#ifndef USE_SCYLLA_DB
    throw std::runtime_error("\nRoute only supported for ScyllaDB\nPlease "
                             "rebuild with USE_SCYLLA_DB flag");
#endif

    if (!std::holds_alternative<Contact>(payload))
        return Lxcode::UNKNOWN_ERROR("Incorrect contact structure");

    const Contact &contact = std::get<Contact>(payload);

    Lxcode code = ScyllaManager::createContact(contact);

    if (code == Lxcode::OK()) {
        printf("Successfully added contact");
        printf("Alpha id: %s", std::get<Contact*>(code.payload)->a_user_id_string().c_str());
        return code;
    }

    return code;
}

RouteManager::~RouteManager() {}
