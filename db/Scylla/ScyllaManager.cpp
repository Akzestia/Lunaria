#include "ScyllaManager.h"
#include <cassandra.h>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>

std::string ScyllaManager::_host = "";
std::string ScyllaManager::_keyspace = "";
std::string ScyllaManager::_password = "";
std::uint16_t ScyllaManager::_port = 0;
std::string ScyllaManager::_user = "";

const std::string getTime() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);

    std::tm tm = *std::localtime(&timestamp);

    std::stringstream ss;

    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

void ScyllaManager::initScyllaManager() {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini("./config.ini", pt);

        _host = pt.get<std::string>("SCYLLA_HOST");
        _port = pt.get<uint16_t>("SCYLLA_PORT");
        _user = pt.get<std::string>("SCYLLA_USER");
        _password = pt.get<std::string>("SCYLLA_PASSWORD");
        _keyspace = pt.get<std::string>("SCYLLA_KEYSPACE");

        std::cout << "ScyllaManager initialized with the following parameters:"
                  << std::endl;
        std::cout << "Host: " << _host << std::endl;
        std::cout << "Port: " << _port << std::endl;
        std::cout << "User: " << _user << std::endl;
        std::cout << "Keyspace: " << _keyspace << std::endl;
        std::cout << "Password: " << _password << std::endl;

    } catch (const boost::property_tree::ptree_error &e) {
        std::cerr << e.what() << std::endl;
        throw std::runtime_error(
            "One or more configuration variables are not set.");
    }
}

Lxcode ScyllaManager::getUser(const SignInRequest &si) {
    const std::string &user_name = si.user_name();
    const std::string &user_password = si.user_password();

    if (user_name.length() < 3 || user_password.length() < 4) {
        return Lxcode::DB_ERROR(DB_ERROR_INVALID_INPUT, "Invalid input");
    }

    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;
    User *u = nullptr;

    try {
        cass_cluster_set_contact_points(cluster, _host.c_str());
        cass_cluster_set_port(cluster, _port);
        cass_cluster_set_credentials(cluster, _user.c_str(), _password.c_str());

        // Connect to the cluster
        connect_future = cass_session_connect(session, cluster);
        if (cass_future_error_code(connect_future) != CASS_OK) {
            cass_future_free(connect_future);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_CONNECTION_FAILED,
                                    "Failed to connect to Scylla cluster");
        }
        cass_future_free(connect_future);

        // Check the user credentials
        statement = cass_statement_new(
            "SELECT user_password FROM lunnaria_service.UserCredentials "
            "WHERE user_name = ?",
            1);
        cass_statement_bind_string(statement, 0, user_name.c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);
        if (!cass_iterator_next(iterator)) {
            std::cerr << "User not found" << std::endl;
            cass_iterator_free(iterator);
            cass_result_free(result);
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_USER_NOT_FOUND, "User not found");
        }

        row = cass_iterator_get_row(iterator);
        const char *retrieved_password;
        size_t retrieved_password_length;
        cass_value_get_string(cass_row_get_column(row, 0), &retrieved_password,
                              &retrieved_password_length);

        if (user_password !=
            std::string(retrieved_password, retrieved_password_length)) {
            cass_iterator_free(iterator);
            cass_result_free(result);
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_INVALID_PASSWORD,
                                    "Invalid password");
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        cass_future_free(result_future);
        cass_statement_free(statement);

        // Retrieve user details from Users table using user_name
        statement = cass_statement_new("SELECT id, display_name, user_name, "
                                       "user_email FROM lunnaria_service.Users "
                                       "WHERE user_name = ?",
                                       1);
        cass_statement_bind_string(statement, 0, user_name.c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);
        if (!cass_iterator_next(iterator)) {
            std::cerr << "User not found in Users table" << std::endl;
            cass_iterator_free(iterator);
            cass_result_free(result);
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_USER_NOT_FOUND,
                                    "User not found in Users table");
        }

        row = cass_iterator_get_row(iterator);
        u = new User();
        const char *display_name;
        size_t display_name_length;
        cass_value_get_string(cass_row_get_column(row, 1), &display_name,
                              &display_name_length);

        const char *user_email;
        size_t user_email_length;
        cass_value_get_string(cass_row_get_column(row, 3), &user_email,
                              &user_email_length);

        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &user_id,
                              &user_id_length);

        u->set_user_id(std::string(user_id, user_id_length));
        u->set_user_name(user_name);
        u->set_user_email(std::string(user_email, user_email_length));
        u->set_display_name(std::string(display_name, display_name_length));

        cass_iterator_free(iterator);
        cass_result_free(result);
        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_session_free(session);
        cass_cluster_free(cluster);

        return Lxcode::OK(u);
    } catch (const std::exception &e) {
        if (session)
            cass_session_free(session);
        if (cluster)
            cass_cluster_free(cluster);
        if (connect_future)
            cass_future_free(connect_future);
        if (statement)
            cass_statement_free(statement);
        if (result_future)
            cass_future_free(result_future);
        if (result)
            cass_result_free(result);
        if (iterator)
            cass_iterator_free(iterator);
        if (u)
            delete u;

        std::cerr << e.what() << std::endl;
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}

Lxcode ScyllaManager::createUser(const SignUpRequest &su) {
    if (su.user_name().length() < 3 || su.user_email().length() < 3 ||
        su.user_password().length() < 3)
        return Lxcode::DB_ERROR(DB_ERROR_INVALID_INPUT, "Invalid input");

    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;
    User *u = nullptr;

    try {
        // Set the contact points and authentication for the Scylla cluster
        cass_cluster_set_contact_points(cluster, _host.c_str());
        cass_cluster_set_port(cluster, _port);
        cass_cluster_set_credentials(cluster, _user.c_str(), _password.c_str());

        // Connect to the cluster
        connect_future = cass_session_connect(session, cluster);
        if (cass_future_error_code(connect_future) != CASS_OK) {
            cass_future_free(connect_future);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_CONNECTION_FAILED,
                                    "Failed to connect to Scylla cluster");
        }

        cass_future_free(connect_future);

        // Check if the user already exists in UserCredentials table by
        // user_name
        statement = cass_statement_new(
            "SELECT user_name FROM lunnaria_service.UserCredentials "
            "WHERE user_name = ?",
            1);
        cass_statement_bind_string(statement, 0, su.user_name().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);
        if (cass_iterator_next(iterator)) {
            std::cerr << "User already exists" << std::endl;
            cass_iterator_free(iterator);
            cass_result_free(result);
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(
                DB_ERROR_USER_EXISTS,
                "User with the same user_name already exists");
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        cass_future_free(result_future);
        cass_statement_free(statement);

        statement = cass_statement_new(
            "SELECT user_email FROM lunnaria_service.ExistingEmails "
            "WHERE user_email = ?",
            1);
        cass_statement_bind_string(statement, 0, su.user_email().c_str());

        std::cout << "Executing query BEFORE EMAIIL" << std::endl;

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);
        if (cass_iterator_next(iterator)) {
            std::cerr << "User already exists" << std::endl;
            cass_iterator_free(iterator);
            cass_result_free(result);
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(
                DB_ERROR_USER_EXISTS,
                "User with the same user_email already exists");
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        cass_future_free(result_future);
        cass_statement_free(statement);

        // Insert into Users table
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        int64_t millis_since_epoch = static_cast<int64_t>(timestamp) * 1000;

        statement = cass_statement_new(
            "INSERT INTO lunnaria_service.Users (id, display_name, user_name, "
            "user_email, user_avatar, online_status, joined_at, last_activity) "
            "VALUES (uuid(), 'User', ?, ?, NULL, 0, ?, ?)",
            4);
        cass_statement_bind_string(statement, 0, su.user_name().c_str());
        cass_statement_bind_string(statement, 1, su.user_email().c_str());
        cass_statement_bind_int64(statement, 2, millis_since_epoch);
        cass_statement_bind_int64(statement, 3, millis_since_epoch);

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        cass_future_free(result_future);
        cass_statement_free(statement);

        std::cout << "Executing query AFTER EMAIIL" << std::endl;
        // Insert into UserCredentials table
        statement =
            cass_statement_new("INSERT INTO lunnaria_service.UserCredentials "
                               "(user_name, user_password, user_email) "
                               "VALUES (?, ?, ?)",
                               3);
        cass_statement_bind_string(statement, 0, su.user_name().c_str());
        cass_statement_bind_string(statement, 1, su.user_password().c_str());
        cass_statement_bind_string(statement, 2, su.user_email().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        cass_future_free(result_future);
        cass_statement_free(statement);

        std::cout << "QUQU\n";

        statement =
            cass_statement_new("INSERT INTO lunnaria_service.ExistingEmails "
                               "(user_email) "
                               "VALUES (?) ",
                               1);
        cass_statement_bind_string(statement, 0, su.user_email().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        std::cout << "FINAL AFTER E INSERT\n";

        u = new User();
        u->set_display_name("User");
        u->set_user_name(su.user_name());
        u->set_user_email(su.user_email());
        u->set_user_avatar("");
        u->set_online_status(0);

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_session_free(session);
        cass_cluster_free(cluster);

        return Lxcode::OK(u);
    } catch (const std::exception &e) {
        if (session)
            cass_session_free(session);
        if (cluster)
            cass_cluster_free(cluster);
        if (connect_future)
            cass_future_free(connect_future);
        if (statement)
            cass_statement_free(statement);
        if (result_future)
            cass_future_free(result_future);
        if (result)
            cass_result_free(result);
        if (iterator)
            cass_iterator_free(iterator);
        if (u)
            delete u;

        std::cerr << e.what() << std::endl;
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}

Lxcode ScyllaManager::createContact(const Contact &contact) {

    printf("Creating %s\n", contact.a_user_name().c_str());
    printf("Creating %s\n", contact.b_user_name().c_str());

    if (contact.a_user_name().empty() || contact.b_user_name().empty() ||
        contact.a_user_name().length() < 3 ||
        contact.b_user_name().length() < 3 ||
        contact.a_user_name().length() > 20 ||
        contact.b_user_name().length() > 20 ||
        contact.a_user_name() == contact.b_user_name()) {
        return Lxcode::DB_ERROR(DB_ERROR_INVALID_INPUT, "Invalid input");
    }

    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;
    std::unique_ptr<Contact> c;

    char a_user_id[CASS_UUID_STRING_LENGTH];
    char b_user_id[CASS_UUID_STRING_LENGTH];

    try {
        // Set the contact points and authentication for the Scylla cluster
        cass_cluster_set_contact_points(cluster, _host.c_str());
        cass_cluster_set_port(cluster, _port);
        cass_cluster_set_credentials(cluster, _user.c_str(), _password.c_str());

        // Connect to the cluster
        connect_future = cass_session_connect(session, cluster);
        if (cass_future_error_code(connect_future) != CASS_OK) {
            cass_future_free(connect_future);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_CONNECTION_FAILED,
                                    "Failed to connect to Scylla cluster");
        }

        cass_future_free(connect_future);

        statement = cass_statement_new("SELECT id FROM lunnaria_service.Users "
                                       "WHERE user_name IN (?, ?)",
                                       2);
        cass_statement_bind_string(statement, 0, contact.a_user_name().c_str());
        cass_statement_bind_string(statement, 1, contact.b_user_name().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);

        int row_count = 0;
        CassUuid id;
        char id_str[CASS_UUID_STRING_LENGTH];

        while (cass_iterator_next(iterator)) {
            row_count++;
            const CassRow *row = cass_iterator_get_row(iterator);

            const CassValue *id_value = cass_row_get_column_by_name(row, "id");

            if (cass_value_get_uuid(id_value, &id) == CASS_OK) {
                row_count == 1 ? cass_uuid_string(id, a_user_id)
                               : cass_uuid_string(id, b_user_id);
                row_count == 1 ? printf("User ID: %s\n", a_user_id)
                               : printf("User ID: %s\n", b_user_id);
            } else {
                std::cerr << "Failed to get UUID from result row" << std::endl;
            }
        }

        printf("\nUser final count %d \n", row_count);

        if (!(row_count == 2)) {
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_result_free(result);
            cass_iterator_free(iterator);
            cass_session_free(session);
            cass_cluster_free(cluster);

            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to find users");
        }

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);
        cass_iterator_free(iterator);

        bool contact_exists = false;

        statement =
            cass_statement_new("SELECT id FROM lunnaria_service.Contacts "
                               "WHERE a_user_id = ? AND b_user_id = ?",
                               2);
        cass_statement_bind_string(statement, 0, contact.a_user_name().c_str());
        cass_statement_bind_string(statement, 1, contact.b_user_name().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);

        if (cass_iterator_next(iterator)) {
            printf("Contact exists\n");
            contact_exists = true;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Contact already exists");
        }

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);
        cass_iterator_free(iterator);

        statement =
            cass_statement_new("SELECT id FROM lunnaria_service.Contacts "
                               "WHERE a_user_id = ? AND b_user_id = ?",
                               2);
        cass_statement_bind_string(statement, 1, contact.a_user_name().c_str());
        cass_statement_bind_string(statement, 0, contact.b_user_name().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);

        if (cass_iterator_next(iterator)) {
            printf("Contact exists\n");
            contact_exists = true;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Contact already exists");
        }

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);
        cass_iterator_free(iterator);

        statement =
            cass_statement_new("INSERT INTO lunnaria_service.Contacts "
                               "(id, a_user_id, b_user_id, created_at) VALUES "
                               "(uuid(), ?, ?, toTimestamp(now()))",
                               2);

        cass_statement_bind_string(statement, 0, a_user_id);
        cass_statement_bind_string(statement, 1, b_user_id);

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_session_free(session);
        cass_cluster_free(cluster);

        c = std::make_unique<Contact>();
        *c->mutable_a_user_id_string() = a_user_id;
        *c->mutable_b_user_id_string() = b_user_id;

        return Lxcode::OK(c.release());
    } catch (const std::exception e) {
        if (session)
            cass_session_free(session);
        if (cluster)
            cass_cluster_free(cluster);
        if (connect_future)
            cass_future_free(connect_future);
        if (statement)
            cass_statement_free(statement);
        if (result_future)
            cass_future_free(result_future);
        if (result)
            cass_result_free(result);
        if (iterator)
            cass_iterator_free(iterator);

        std::cout << e.what() << "\n";
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}

Lxcode ScyllaManager::createServer(const Server &server) {
    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;
    Server *s = nullptr;

    try {
        // Set the contact points and authentication for the Scylla cluster
        cass_cluster_set_contact_points(cluster, _host.c_str());
        cass_cluster_set_port(cluster, _port);
        cass_cluster_set_credentials(cluster, _user.c_str(), _password.c_str());

        // Connect to the cluster
        connect_future = cass_session_connect(session, cluster);
        if (cass_future_error_code(connect_future) != CASS_OK) {
            cass_future_free(connect_future);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_CONNECTION_FAILED,
                                    "Failed to connect to Scylla cluster");
        }

        cass_future_free(connect_future);

        int row_count = 0;
        while (cass_iterator_next(iterator)) {
            row_count++;
            row = cass_iterator_get_row(iterator);

            // Retrieve the user IDs
            const char *user_id;
            size_t user_id_length;
            cass_value_get_string(cass_row_get_column(row, 0), &user_id,
                                  &user_id_length);
            if (row_count == 1) {
                // a_user_id = std::string(user_id, user_id_length);
            } else if (row_count == 2) {
                // b_user_id = std::string(user_id, user_id_length);
            }
        }

        // Check if both user IDs were found
        if (row_count == 2) {
            // Use the retrieved user IDs to create the contact
            // ...
        } else {
            // Handle the case where the expected number of users was not found
            // ...
        }

        // Clean up resources
        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_session_free(session);
        cass_cluster_free(cluster);

        return Lxcode::OK(s);
    } catch (const std::exception e) {

        if (s)
            delete s;
        if (session)
            cass_session_free(session);
        if (cluster)
            cass_cluster_free(cluster);
        if (connect_future)
            cass_future_free(connect_future);
        if (statement)
            cass_statement_free(statement);
        if (result_future)
            cass_future_free(result_future);
        if (result)
            cass_result_free(result);
        if (iterator)
            cass_iterator_free(iterator);

        std::cout << e.what() << "\n";
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}

Lxcode ScyllaManager::getContacts(const char *&usser_id) {
    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;

    try {

        // Set the contact points and authentication for the Scylla cluster
        cass_cluster_set_contact_points(cluster, _host.c_str());
        cass_cluster_set_port(cluster, _port);
        cass_cluster_set_credentials(cluster, _user.c_str(), _password.c_str());

        // Connect to the cluster
        connect_future = cass_session_connect(session, cluster);
        if (cass_future_error_code(connect_future) != CASS_OK) {
            cass_future_free(connect_future);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_CONNECTION_FAILED,
                                    "Failed to connect to Scylla cluster");
        }

        cass_future_free(connect_future);

        statement = cass_statement_new(
            "SELECT user_password FROM lunnaria_service.UserCredentials "
            "WHERE user_name = ?",
            1);
        cass_statement_bind_string(statement, 0, usser_id);

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute query");
        }

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);
        if (!cass_iterator_next(iterator)) {
            std::cerr << "User not found" << std::endl;
            cass_iterator_free(iterator);
            cass_result_free(result);
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_USER_NOT_FOUND, "User not found");
        }

        return Lxcode::OK();
    } catch (const std::exception e) {
        if (session)
            cass_session_free(session);
        if (cluster)
            cass_cluster_free(cluster);
        if (connect_future)
            cass_future_free(connect_future);
        if (statement)
            cass_statement_free(statement);
        if (result_future)
            cass_future_free(result_future);
        if (result)
            cass_result_free(result);
        if (iterator)
            cass_iterator_free(iterator);

        std::cout << e.what() << "\n";
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}
