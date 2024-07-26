#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#include "../../MsQuic/Linux_x64/include/msquic.h"
#include "../../proto/build/user.pb.h"
#include <condition_variable>
#include <mutex>
#include <sys/stat.h>
#include <unordered_map>

class ConnectionManager {
  protected:
    static std::unordered_map<HQUIC, User> *getUsers();
    static std::unordered_map<HQUIC, HQUIC> *getConnections();
    static bool addUser(HQUIC, User);
    static bool updateUser(User);
    static bool removeUser(User);
    static bool removeUser(HQUIC);
    static HQUIC getConenction(User);


    static bool addConnection(HQUIC, HQUIC);
    static bool closeConnection(HQUIC, const QUIC_API_TABLE &);

    virtual ~ConnectionManager();

  private:
    static std::unordered_map<HQUIC, User> *users;
    static std::unordered_map<HQUIC, HQUIC> *connections;
    static std::condition_variable user_cv;
    static std::mutex user_mutex;
    static std::condition_variable connection_cv;
    static std::mutex connection_mutex;

    ConnectionManager() = default;
    friend class QuicServer;
};
#endif