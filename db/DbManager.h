#pragma once
#include <pqxx/pqxx> 
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
#include "../proto/build/user.pb.h"


class DbManager{
public:
    static void test();
    static void getUser(std::string, uint8_t);
    static void getMessages(const User&);
    static void getContacts(const User&);
    static void getGraphs(const User&);
private:
    static std::string getConnectionString();
};