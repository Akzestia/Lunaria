#pragma once
#include "../../MsQuic/include/msquic.h"
#include "../../proto/build/user.pb.h"
#include <condition_variable>
#include <unordered_map>
#include <mutex>

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
    static std::condition_variable user_cv;
    static std::mutex user_mutex;
};