#pragma once
#include "../proto/build/user.pb.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <pqxx/pqxx>

class DbManager {
  public:
    static void test();
  protected:
    static void getUser(std::string, uint8_t);
    static void getMessages(const User &);
    static void getContacts(const User &);
    static void getGraphs(const User &);
    virtual ~DbManager();
  private:
    static std::string getConnectionString();
};