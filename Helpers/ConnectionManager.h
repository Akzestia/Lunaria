#pragma once
#include "../MsQuic/include/msquic.h"
#include "../proto/build/user.pb.h"
#include <unordered_map>

class ConnectionManager {
  protected:
    static std::unordered_map<HQUIC, User>* getUsers();
    static bool addUser(HQUIC, User);
    static bool updateUser(User);
    static bool removeUser(User);
    static bool removeUser(HQUIC);
    static HQUIC getConenction(User);
    virtual ~ConnectionManager();
  private:
    static std::unordered_map<HQUIC, User>* users;
};