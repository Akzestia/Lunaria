#include "../MsQuic/include/msquic.h"
#include "../proto/user.pb.h"
#include <unordered_map>

class ConnectionManager {
  public:
    static std::unordered_map<HQUIC, User>* getUsers();
    static bool addUser(HQUIC, User);
    static bool updateUser(User);
    static bool removeUser(User);
    static HQUIC getConenction(User);
  private:
    static std::unordered_map<HQUIC, User>* users;
};