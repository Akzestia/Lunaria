#pragma once
#include "../MsQuic/include/msquic.h"
#include "../proto/build/user.pb.h"
#include <unordered_map>

class ConnectionManager {
  public:
    static std::unordered_map<HQUIC, User>* getUsers();
    static bool addUser(HQUIC, User);
    static bool updateUser(User);
    static bool removeUser(User);
    static HQUIC getConenction(User);
    ~ConnectionManager();
  private:
    static std::unordered_map<HQUIC, User>* users;
};