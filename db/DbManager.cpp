#include "DbManager.h"
#include <cstring>
#include <sys/types.h>

void DbManager::test() {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return;
        }

        pqxx::nontransaction nontransaction(connection);

        pqxx::result result = nontransaction.exec("SELECT * FROM users;");

        pqxx::result::const_iterator row_it = result.begin();
        pqxx::result::const_iterator row_end = result.end();

        for (; row_it != row_end; ++row_it) {
            const pqxx::result::const_iterator row = *row_it;
            std::cout << row[0].c_str() << std::endl;
            std::cout << row[1].c_str() << std::endl;
            std::cout << row[2].c_str() << std::endl;
        }

        connection.close();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

bool DbManager::getMessages(const User& u, std::vector<uint8_t>* output){
    try{
        const std::string connection_str = DbManager::getConnectionString();

        const char* buffer = "test";
        memcpy(&output, buffer, 4);
        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        pqxx::nontransaction nontransaction(connection);

        //Todo
        pqxx::result result = nontransaction.exec("SELECT * FROM messages;");

        pqxx::result::const_iterator row_it = result.begin();
        pqxx::result::const_iterator row_end = result.end();

        for (; row_it != row_end; ++row_it) {
            const pqxx::result::const_iterator row = *row_it;
            std::cout << row[0].c_str() << std::endl;
            std::cout << row[1].c_str() << std::endl;
            std::cout << row[2].c_str() << std::endl;
        }

        connection.close();
        return true;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}


bool DbManager::getContacts(const User& u, std::vector<uint8_t>* output){
       try{
         const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        pqxx::nontransaction nontransaction(connection);

        //Todo
        pqxx::result result = nontransaction.exec("SELECT * FROM contacts;");

        pqxx::result::const_iterator row_it = result.begin();
        pqxx::result::const_iterator row_end = result.end();

        for (; row_it != row_end; ++row_it) {
            const pqxx::result::const_iterator row = *row_it;
            std::cout << row[0].c_str() << std::endl;
            std::cout << row[1].c_str() << std::endl;
            std::cout << row[2].c_str() << std::endl;
        }

        connection.close();
        return true;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}


bool DbManager::getGraphs(const User& u, std::vector<uint8_t>* output){
        try{
         const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        pqxx::nontransaction nontransaction(connection);

        //Todo
        pqxx::result result = nontransaction.exec("SELECT * FROM graphs;");

        pqxx::result::const_iterator row_it = result.begin();
        pqxx::result::const_iterator row_end = result.end();

        for (; row_it != row_end; ++row_it) {
            const pqxx::result::const_iterator row = *row_it;
            std::cout << row[0].c_str() << std::endl;
            std::cout << row[1].c_str() << std::endl;
            std::cout << row[2].c_str() << std::endl;
        }

        connection.close();
        return true;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}


#pragma region POST
bool DbManager::addUser (const User& user){
    return true;
}

bool DbManager::addMessage(const Message& message){
    return true;
}

bool DbManager::addContact (const Contact&  contact){
    return true;
}
#pragma endregion


#pragma region PUT
bool DbManager::updateUser (const User& user){
    return true;
}

bool DbManager::updateMessage(const Message& message){
    return true;
}

bool DbManager::updateContact (const Contact& contact){
    return true;
}
#pragma endregion

#pragma region DELETE
bool DbManager::deleteUser (const User& user){
    return true;
}

bool DbManager::deleteMessage(const Message& message){
    return true;
}

bool DbManager::deleteContact (const Contact& contact){
    return true;
}
#pragma endregion


std::string DbManager::getConnectionString() {
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("./db/config.ini", pt);

    try {
        std::string db_host = pt.get<std::string>("DB_HOST");
        std::string db_user = pt.get<std::string>("DB_USER");
        std::string db_name = pt.get<std::string>("DB_NAME");
        std::string db_password = pt.get<std::string>("DB_PASSWORD");

        std::string connection_string = "host=" + db_host + " user=" + db_user +
                                        " dbname=" + db_name +
                                        " password=" + db_password;
        return connection_string;
    } catch (const boost::property_tree::ptree_error &e) {
        throw std::runtime_error("One or more configuration variables are not set.");
    }
}

DbManager::~DbManager(){

}