#pragma once
#include <pqxx/pqxx> 
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>

class DbManager{

public:
    void test();
    static std::string getConnectionString();
private:
};