
#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H

// #include "../db/Postgress/DbManager.h"
#include "../Helpers/AuthManager/AuthManager.h"
#include "../db/Scylla/ScyllaManager.h"
#include "../proto/build/auth.pb.h"
#include "../proto/build/contact.pb.h"
#include "../proto/build/encrypt_key.pb.h"
#include "../proto/build/invite_link.pb.h"
#include "../proto/build/message.pb.h"
#include "../proto/build/report.pb.h"
#include "../proto/build/rpc_body.pb.h"
#include "../proto/build/rpc_request.pb.h"
#include "../proto/build/server.pb.h"
#include "../proto/build/sign_in.pb.h"
#include "../proto/build/sign_up.pb.h"
#include "../proto/build/user.pb.h"
#include "../proto/build/vpn_graph.pb.h"
#include "../proto/build/wrapper.pb.h"
#include <functional>
#include <google/protobuf/arena.h>
#include <set>
#include <unordered_map>

using Arena = google::protobuf::Arena;

using Payload =
    std::variant<User, Message, Contact, Auth, Vpn_graph, Server, Report,
                 Invite_link, Encrypt_key, Sign_up, Sign_in>;
using RouteFunction = std::function<Lxcode(const Payload &)>;

class RouteManager {
  public:
  protected:
    static void InitScyllaDb();
    static Lxcode proccesRoute(const Wrapper &);
    virtual ~RouteManager();

  private:
    static Lxcode handleReport(const Payload &, Arena &);
    static Lxcode handleSignUp(const SignUpRequest &, Arena &);
    static Lxcode handleSignIn(const SignInRequest &, Arena &);

    static Lxcode getMessages(const char *&, Arena &);
    static Lxcode getContacts(const char *&, Arena &);
    static Lxcode getServers(const char *&, Arena &);

    static Lxcode createContact(const Payload &, Arena &);

    static Lxcode updateUser(const Payload &, Arena &);
    static Lxcode updateServer(const Payload &, Arena &);

    // RouteManager() : DbManager() {};

    friend class PeerHandler;
};

#endif
