#ifndef SCYLLA_MANAGER_H
#define SCYLLA_MANAGER_H

#include "../../error-manager/ErrorManager.h"
#include "../../proto/build/auth.pb.h"
#include "../../proto/build/contact.pb.h"
#include "../../proto/build/message.pb.h"
#include "../../proto/build/sign_in.pb.h"
#include "../../proto/build/user.pb.h"
#include "../../proto/build/server.pb.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <condition_variable>
#include <cstdint>
#include <iostream>

class ScyllaManager {
  protected:
    static void initScyllaManager();
#pragma region GET
    static Lxcode getUser(const Sign_in &);
    // Lxcode getMembers(const Server &);
    static Lxcode getContacts(const char* &);
    static Lxcode getMessages(const User &);
    static Lxcode getServers(const User &);
#pragma endregion

#pragma region POST
    static Lxcode createUser(const Sign_up &);
    static Lxcode createContact(const Contact &);
    static Lxcode createMessage(const Message &);
    static Lxcode createServer(const Server &);
#pragma endregion

#pragma region PUT
    static Lxcode updateUser(const User &);
    static Lxcode updateContact(const Contact &);
    static Lxcode updateMessage(const Message &);
    static Lxcode updateServer(const Auth &);
#pragma endregion

  private:
    static std::string _host;
    static std::uint16_t _port;
    static std::string _user;
    static std::string _password;
    static std::string _keyspace;

    ScyllaManager() = default;
    friend class QuicServer;
    friend class RouteManager;
};

#endif // SCYLLA_MANAGER_H
