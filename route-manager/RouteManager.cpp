#include "RouteManager.h"

Lxcode RouteManager::handleAuth(const Payload &payload) {
    Lxcode return_code;
    return_code.error_code = 0x00;
    return_code.is_successful = true;
    if (std::holds_alternative<Auth>(payload)) {
        const Auth &auth = std::get<Auth>(payload);
        
        return return_code;
    } else {
        return_code.error_code = 0x01;
        return_code.is_successful = false;
        return return_code;
    }
}

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

Lxcode RouteManager::handleSignUp(const Payload &payload) {
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

std::unordered_map<uint8_t, RouteFunction> *RouteManager::routes =
    new std::unordered_map<uint8_t, RouteFunction>(
        {{1, RouteManager::handleAuth}, {2, RouteManager::handleReport}});

RouteManager::~RouteManager() {
    if (routes)
        delete routes;
}