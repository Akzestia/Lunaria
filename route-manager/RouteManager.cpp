#include "RouteManager.h"
#include "../error-manager/ErrorManager.h"
#include <cstdio>
#include <variant>

void RouteManager::InitScyllaDb() { ScyllaManager::initScyllaManager(); }

Lxcode RouteManager::handleReport(const Payload &payload, Arena &) {
    Lxcode return_code = Lxcode::OK();
    if (std::holds_alternative<Report>(payload)) {
        const Report &report = std::get<Report>(payload);

        return return_code;
    } else {
        return_code.error_code = 0x01;
        return_code.is_successful = false;
        return return_code;
    }
}

Lxcode RouteManager::handleSignUp(const SignUpRequest &sign_up, Arena &arena) {

    Lxcode return_code = Lxcode::OK();

#ifdef USE_SCYLLA_DB
    return_code = ScyllaManager::createUser(sign_up, arena);
#else

#endif

    if (return_code == Lxcode::OK()) {
        return_code.response = AuthManager::generateToken(
            std::get<User *>(return_code.payload)->user_name().c_str(),
            std::get<User *>(return_code.payload)->user_password().c_str());
        return return_code;
    }

    return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED, "");
    ;
}

Lxcode RouteManager::handleSignIn(const SignInRequest &si, Arena &arena) {
    Lxcode return_code = Lxcode::OK();
#ifdef USE_SCYLLA_DB
    printf("Before return code");
    return_code = ScyllaManager::getUser(si, arena);
    printf("After return code");
#else
    // return_code = DbManager::getUser(si);
#endif

    if (return_code == Lxcode::OK()) {
        return return_code;
    }

    return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED, "");
    ;
}

Lxcode RouteManager::getMessages(const char *&user_id, Arena &) {
    return Lxcode::UNKNOWN_ERROR("Not implemented");
}

Lxcode RouteManager::createContact(const Payload &payload, Arena &arena) {

#ifndef USE_SCYLLA_DB
    throw std::runtime_error("\nRoute only supported for ScyllaDB\nPlease "
                             "rebuild with USE_SCYLLA_DB flag");
#endif

    if (!std::holds_alternative<Contact>(payload))
        return Lxcode::UNKNOWN_ERROR("Incorrect contact structure");

    const Contact &contact = std::get<Contact>(payload);

    Lxcode code = ScyllaManager::createContact(contact, arena);

    if (code == Lxcode::OK()) {
        printf("Successfully added contact");
        printf("Alpha id: %s",
               std::get<Contact *>(code.payload)->a_user_id_string().c_str());
        return code;
    }

    return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED, "");
}

Lxcode RouteManager::getContacts(const char *&user_id, Arena &arena) {
    Lxcode code = ScyllaManager::getContacts(user_id, arena);

    if (code == Lxcode::OK()) {
        return code;
    }

    return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED, "");
}

RouteManager::~RouteManager() {}
