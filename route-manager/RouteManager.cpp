#include "RouteManager.h"

bool RouteManager::handleAuth(const Payload &payload) {
    if (std::holds_alternative<Auth>(payload)) {
        const Auth &auth = std::get<Auth>(payload);

        return true;
    }
    return false;
}

bool RouteManager::handleReport(const Payload &payload) {
    if (std::holds_alternative<Report>(payload)) {
        const Report &report = std::get<Report>(payload);

        return true;
    }
    return true;
}

std::unordered_map<uint8_t, RouteFunction> *RouteManager::routes =
    new std::unordered_map<uint8_t, RouteFunction>(
        {{1, RouteManager::handleAuth}});

RouteManager::~RouteManager() {
    if (routes)
        delete routes;
}