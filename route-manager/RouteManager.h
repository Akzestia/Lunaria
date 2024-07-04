#pragma once
#include "../db/DbManager.h"
#include "../proto/build/auth.pb.h"
#include "../proto/build/contact.pb.h"
#include "../proto/build/encrypt_key.pb.h"
#include "../proto/build/invite_link.pb.h"
#include "../proto/build/message.pb.h"
#include "../proto/build/report.pb.h"
#include "../proto/build/server.pb.h"
#include "../proto/build/user.pb.h"
#include "../proto/build/vpn_graph.pb.h"
#include "../proto/build/wrapper.pb.h"
#include <functional>
#include <set>
#include <unordered_map>

using Payload = std::variant<User, Message, Contact, Auth, Vpn_graph, Server,
                             Report, Invite_link, Encrypt_key>;
using RouteFunction = std::function<bool(const Payload &)>;

class RouteManager : protected DbManager {
  public:
  protected:
    static bool proccesRoute(const Wrapper &);
    virtual ~RouteManager();

  private:
    static bool handleAuth(const Payload &);
    static bool handleReport(const Payload &);

    static bool getMessages(const Payload &, std::set<Message> &);
    static bool getContacts(const Payload &, std::set<User> &);
    static bool getServers(const Payload &, std::set<Server> &);

    static bool updateUser(const Payload &);
    static bool updateServer(const Payload &);

    static std::unordered_map<uint8_t, RouteFunction> *routes;
};