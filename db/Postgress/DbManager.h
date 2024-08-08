#ifndef DB_MANAGER_H
#define DB_MANAGER_H
#include "../../proto/build/user.pb.h"
#include "../../proto/build/message.pb.h"
#include "../../proto/build/contact.pb.h"
#include "../../error-manager/ErrorManager.h"
#include "../../proto/build/auth.pb.h"
#include "../../proto/build/sign_in.pb.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <condition_variable>
#include <iostream>
#include <pqxx/pqxx>

class DbManager {
  public:
    static void test();
  protected:

    #pragma region GET
    static Lxcode getUser(const Sign_in &);
    static bool getMessages(const User &, std::vector<uint8_t>*);
    static bool getContacts(const User &, std::vector<uint8_t>*);
    static bool getGraphs(const User &, std::vector<uint8_t>*);
    #pragma endregion

    #pragma region POST
    static Lxcode addUser(const User&);
    static bool addMessage(const Message&);
    static bool addContact(const Contact&);
    #pragma endregion

    #pragma region PUT
    static bool updateUser(const User&);
    static bool updateMessage(const Message&);
    static bool updateContact(const Contact&);
    #pragma endregion

    #pragma region DELETE
    static bool deleteUser(const User&);
    static bool deleteMessage(const Message&);
    static bool deleteContact(const Contact&);
    #pragma endregion

    virtual ~DbManager();
  private:
    static std::string getConnectionString();
    DbManager() = default;
    friend class QuicServer;
    friend class RequestManager;
    friend class RouteManager;
};

#endif
