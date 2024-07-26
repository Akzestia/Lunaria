#include "ConnectionManager.h"

std::unordered_map<HQUIC, User> *ConnectionManager::getUsers() { return users; }
std::unordered_map<HQUIC, HQUIC> *ConnectionManager::getConnections() { return connections; }
std::unordered_map<HQUIC, User> *ConnectionManager::users = new std::unordered_map<HQUIC, User>();
std::unordered_map<HQUIC, HQUIC> *ConnectionManager::connections = new std::unordered_map<HQUIC, HQUIC>();

bool ConnectionManager::addConnection(HQUIC connection, HQUIC stream){
    if((*connections).find(stream) != (*connections).end()){
        printf("\nconnection already exists\n");
        return false;
    }
    (*connections)[stream] = connection;
    printf("\nconnection was successfully added\n");
    return true;
}

bool ConnectionManager::closeConnection(HQUIC stream, const QUIC_API_TABLE &table){
    if((*connections).find(stream) == (*connections).end()){
        printf("\nconnection wasn't found\n");
        return false;
    }
    HQUIC connection = (*connections)[stream];
    if(stream){
        table.ConnectionShutdown(connection, QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
        table.StreamShutdown(stream, QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL, 0);
        table.StreamClose(stream);
    }
    size_t erased = (*connections).erase(stream);
    if(erased <= 0){
        printf("\nconnection wasn't removed\n");
        return false;
    }
    printf("\nconnection was successfully removed\n");
    return true;
}


HQUIC ConnectionManager::getConenction(User user) {

    auto start = (*users).begin();
    auto end = (*users).end();

    for (; start != end; start++)
        if (start->second.user_email() == user.user_email())
            return start->first;
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
bool ConnectionManager::removeUser(HQUIC connection) {
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