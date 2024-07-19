
#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H

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
#include "../proto/build/sign_in.pb.h"
#include "../proto/build/sign_up.pb.h"
#include "../Helpers/AuthManager/AuthManager.h"
#include <functional>
#include <set>
#include <unordered_map>

using Payload = std::variant<User, Message, Contact, Auth, Vpn_graph, Server,
                             Report, Invite_link, Encrypt_key, Sign_up, Sign_in>;
using RouteFunction = std::function<Lxcode(const Payload &)>;

class RouteManager : protected DbManager {
  public:
  protected:
    static Lxcode proccesRoute(const Wrapper &);
    virtual ~RouteManager();
  private:

    static Lxcode handleAuth(const Payload &);
    static Lxcode handleReport(const Payload &);
    static Lxcode handleSignUp(const Payload &);
    static Lxcode handleSignIn(const Payload &);

    static Lxcode getMessages(const Payload &, std::set<Message> &);
    static Lxcode getContacts(const Payload &, std::set<User> &);
    static Lxcode getServers(const Payload &, std::set<Server> &);

    static Lxcode updateUser(const Payload &);
    static Lxcode updateServer(const Payload &);

    static std::unordered_map<uint8_t, RouteFunction> *routes;

    RouteManager() : DbManager() {};

    friend class PeerHandler;
};

#endif