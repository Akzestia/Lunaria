#include "ConnectionManager.h"

std::unordered_map<HQUIC, User> *ConnectionManager::getUsers() { return users; }
std::unordered_map<HQUIC, User> *ConnectionManager::users = new std::unordered_map<HQUIC, User>();

HQUIC ConnectionManager::getConenction(User user) {
    for (auto &pair : (*users))
        if (pair.second.email() == user.email())
            return pair.first;
    return nullptr;
}
bool ConnectionManager::addUser(HQUIC connection, User user) {

    if ((*users).find(connection) != (*users).end()) {
        printf("\nuser already exists\n");
        return false;
    }

    (*users)[connection] = user;
    printf("\nuser was successfully added\n");
    return true;
}
bool ConnectionManager::removeUser(User user) {

    HQUIC connection = ConnectionManager::getConenction(user);
    if (!connection) {
        printf("\nuser wasn't found");
        return false;
    }
    size_t erased = (*users).erase(connection);
    if (erased <= 0) {
        printf("\nuser wasn't removed");
        return false;
    }
    printf("\nuser was successfully removed\n");
    return true;
}
bool ConnectionManager::updateUser(User user) {
    HQUIC connection = ConnectionManager::getConenction(user);
    if (!connection) {
        printf("\nuser wasn't found");
        return false;
    }

    (*users)[connection] = user;
    printf("\nuser was successfully updated\n");
    return true;
}

ConnectionManager::~ConnectionManager(){
    if(users) delete users;
}