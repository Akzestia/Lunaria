#include "../MsQuic/include/msquic.h"
#include "../proto/user.pb.h"
#include <unordered_map>

class ConnectionManager {
  public:
    std::unordered_map<HQUIC, User> getUsers();
    bool addUser(HQUIC, User);
    bool updateUser(User);
    bool removeUser(User);
  private:
    std::unordered_map<HQUIC, User> users;
};