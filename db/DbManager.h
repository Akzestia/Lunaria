#pragma once
#include "../proto/build/user.pb.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <condition_variable>
#include <iostream>
#include <pqxx/pqxx>

class DbManager {
  public:
    static void test();
  protected:
    static void getUser(std::string, uint8_t);
    static bool getMessages(const User &);
    static bool getContacts(const User &);
    static bool getGraphs(const User &);
    virtual ~DbManager();
  private:
    static std::string getConnectionString();
};