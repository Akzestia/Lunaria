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

#pragma region GET
Lxcode DbManager::getUser(const Auth &auth, User *output) {

    if (!auth.has_sign_in()) {
        return {false, AUTH_ERROR_INCORRECT_PAYLOAD_FORMAT};
    }

    const std::string user_name = auth.sign_in().user_name();
    const std::string user_password = auth.sign_in().user_password();

    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return {false, DB_ERROR_CONNECTION_FAILED};
        }

        pqxx::nontransaction nontransaction(connection);

        const std::string query = "SELECT FIRST FROM Users WHERE user_password "
                                  "= $1 AND user_name = $2;";

        pqxx::result result =
            nontransaction.exec_params(query, user_password, user_name);

        pqxx::result::const_iterator row_it = result.begin();
        pqxx::result::const_iterator row_end = result.end();

        for (; row_it != row_end; ++row_it) {
            const pqxx::result::const_iterator row = *row_it;
            std::cout << row[0].c_str() << std::endl;
            std::cout << row[1].c_str() << std::endl;
        }

        connection.close();
        return {true, SUCCESS};
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return {false, DB_ERROR_STD_EXCEPTION};
    }
};

bool DbManager::getMessages(const User &u, std::vector<uint8_t> *output) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        const char *buffer = "test";
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

        // Todo
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
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::getContacts(const User &u, std::vector<uint8_t> *output) {
    try {
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

        // Todo
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
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::getGraphs(const User &u, std::vector<uint8_t> *output) {
    try {
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

        // Todo
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
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}
#pragma endregion

#pragma region POST
bool DbManager::addUser(const User &user) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        pqxx::work txn(connection);

        std::string query =
            "INSERT INTO Users (user_display_name, user_name, user_email, "
            "user_avatar, user_password, online_status) VALUES (" +
            txn.quote(user.display_name()) + ", " +
            txn.quote(user.user_name()) + ", " + txn.quote(user.user_email()) +
            ", " + txn.quote(user.user_avatar()) + ", " +
            txn.quote(user.user_password()) + ", " +
            txn.quote(user.online_status()) + ");";

        txn.exec(query);

        txn.commit();

        std::cout << "\nUser added successfully." << std::endl;

        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::addMessage(const Message &message) {

    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        pqxx::work txn(connection);

        std::string query =
            "INSERT INTO Messages (sender_id, receiver_id, text_content, "
            "byte_content) VALUES (" +
            txn.quote(message.sender_id()) + ", " +
            txn.quote(message.receiver_id()) + ", " +
            txn.quote(message.text_content()) + ", " +
            txn.quote(message.byte_content()) + ");";

        txn.exec(query);

        txn.commit();

        std::cout << "\nMessage added successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::addContact(const Contact &contact) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        pqxx::work txn(connection);

        std::string query =
            "INSERT INTO Contacts (a_user_id, b_user_id) VALUES (" +
            txn.quote(contact.a_user_id()) + ", " +
            txn.quote(contact.b_user_id()) + ");";

        txn.exec(query);

        txn.commit();

        std::cout << "\nContact added successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}
#pragma endregion

#pragma region PUT
bool DbManager::updateUser(const User &user) {

    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        // Todo

        std::cout << "\nUser updated successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::updateMessage(const Message &message) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        // Todo

        std::cout << "\nMessage updated successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::updateContact(const Contact &contact) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        // Todo

        std::cout << "\nContact updated successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}
#pragma endregion

#pragma region DELETE
bool DbManager::deleteUser(const User &user) {

    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        // Todo

        std::cout << "\nUser deleted successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::deleteMessage(const Message &message) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        // Todo

        std::cout << "\nMessage deleted successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool DbManager::deleteContact(const Contact &contact) {
    try {
        const std::string connection_str = DbManager::getConnectionString();

        pqxx::connection connection(connection_str);

        if (connection.is_open()) {
            std::cout << "Connected to database successfully: "
                      << connection.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return false;
        }

        // Todo

        std::cout << "\nContact deleted successfully." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
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
        throw std::runtime_error(
            "One or more configuration variables are not set.");
    }
}

DbManager::~DbManager() {}