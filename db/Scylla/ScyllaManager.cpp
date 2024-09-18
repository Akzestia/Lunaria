#include "ScyllaManager.h"
#include "../../Helpers/Encryption/EncryptionManager.h"
#include <cassandra.h>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <exception>
#include <google/protobuf/arena.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

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

Lxcode ScyllaManager::getUser(const SignInRequest &si, Arena &arena) {
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

        // Check the user credentialsuser_pas
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

        std::string output;
        EncryptionManager::ToSHA256(user_password, output);

        if (output !=
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

        CassUuid user_id_uuid;
        cass_value_get_uuid(cass_row_get_column(row, 0), &user_id_uuid);

        char user_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(user_id_uuid, user_id_str);

        u = google::protobuf::Arena::Create<User>(&arena);
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

        std::cout << "Izu (UUID): " << user_id_str << "\n";

        u->set_user_id(std::string(user_id_str));
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

        std::cerr << e.what() << std::endl;
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}

Lxcode ScyllaManager::createUser(const SignUpRequest &su, Arena &arena) {
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

    CassUuidGen *uuid_gen = cass_uuid_gen_new();
    CassUuid uuid;
    cass_uuid_gen_random(uuid_gen, &uuid);
    cass_uuid_gen_free(uuid_gen);

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

        char uuid_str[37]; // UUID string representation is 36 bytes plus null
                           // terminator
        cass_uuid_string(uuid, uuid_str);

        std::cout << "UUID() = " << uuid_str << "\n";

        statement = cass_statement_new(
            "INSERT INTO lunnaria_service.Users (id, display_name, user_name, "
            "user_email, user_avatar, online_status, joined_at, last_activity) "
            "VALUES (?, 'User', ?, ?, NULL, 0, ?, ?)",
            5);
        cass_statement_bind_uuid(statement, 0, uuid);
        cass_statement_bind_string(statement, 1, su.user_name().c_str());
        cass_statement_bind_string(statement, 2, su.user_email().c_str());
        cass_statement_bind_int64(statement, 3, millis_since_epoch);
        cass_statement_bind_int64(statement, 4, millis_since_epoch);

        std::cout << "After UUID\n";

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

        std::string output;
        EncryptionManager::ToSHA256(su.user_password(), output);
        statement =
            cass_statement_new("INSERT INTO lunnaria_service.UserCredentials "
                               "(id, user_name, user_password, user_email) "
                               "VALUES (uuid(), ?, ?, ?)",
                               3);
        cass_statement_bind_string(statement, 0, su.user_name().c_str());
        cass_statement_bind_string(statement, 1, output.c_str());
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

        cass_future_free(result_future);
        cass_statement_free(statement);

        std::cout << "Before ByID\n";

        statement = cass_statement_new("INSERT INTO lunnaria_service.UsersByID "
                                       "(id, user_name) "
                                       "VALUES (?, ?)",
                                       2);
        cass_statement_bind_uuid(statement, 0, uuid);
        cass_statement_bind_string(statement, 1, su.user_name().c_str());

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

        u = google::protobuf::Arena::Create<User>(&arena);
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

        std::cerr << e.what() << std::endl;
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}

Lxcode ScyllaManager::createContact(const Contact &contact, Arena &arena) {

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
    Contact *c = nullptr;

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

        c = google::protobuf::Arena::Create<Contact>(&arena);
        *c->mutable_a_user_id_string() = a_user_id;
        *c->mutable_b_user_id_string() = b_user_id;

        return Lxcode::OK(c);
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
    Server *s;

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

        return Lxcode::OK(std::move(s));
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

Lxcode ScyllaManager::getContacts(const char *&usser_id, Arena &arena) {

    CassUuid usser_id_uuid;
    CassError rc = cass_uuid_from_string(usser_id, &usser_id_uuid);
    if (rc != CASS_OK) {
        return Lxcode::DB_ERROR(DB_ERROR_INVALID_INPUT,
                                "Failed to convert user_id to UUID");
    }

    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;

    ArenaVector<User> *users = nullptr;

    std::cout << "Fetching contact data with id [" << usser_id << "]\n";

    CassUuid id;
    char id_str[CASS_UUID_STRING_LENGTH];
    std::map<std::string, char> contacts;
    std::vector<std::string> v_user_names;

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
            "SELECT a_user_id, b_user_id FROM lunnaria_service.Contacts "
            "WHERE a_user_id = ?",
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

        // Debug: Confirm query executed successfully
        std::cout << "Query executed successfully for a_user_id: " << usser_id
                  << std::endl;

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            row = cass_iterator_get_row(iterator);
            const CassValue *a_id =
                cass_row_get_column_by_name(row, "a_user_id");
            const CassValue *b_id =
                cass_row_get_column_by_name(row, "b_user_id");

            const char *a_id_str;
            size_t a_id_length;
            if (cass_value_get_string(a_id, &a_id_str, &a_id_length) ==
                CASS_OK) {
                std::string a_id_value(a_id_str, a_id_length);
                std::cout << "a_user_id: " << a_id_value << std::endl;
                if (usser_id != a_id_value)
                    contacts[a_id_value] = 'a';
            }

            const char *b_id_str;
            size_t b_id_length;
            if (cass_value_get_string(b_id, &b_id_str, &b_id_length) ==
                CASS_OK) {
                std::string b_id_value(b_id_str, b_id_length);
                std::cout << "b_user_id: " << b_id_value << std::endl;
                if (usser_id != b_id_value)
                    contacts[b_id_value] = 'b';
            }
        }

        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_future_free(result_future);
        cass_statement_free(statement);

        statement = cass_statement_new(
            "SELECT a_user_id, b_user_id FROM lunnaria_service.Contacts "
            "WHERE b_user_id = ?",
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

        // Debug: Confirm query executed successfully
        std::cout << "Query executed successfully for b_user_id: " << usser_id
                  << std::endl;

        result = cass_future_get_result(result_future);
        iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            row = cass_iterator_get_row(iterator);
            const CassValue *a_id =
                cass_row_get_column_by_name(row, "a_user_id");
            const CassValue *b_id =
                cass_row_get_column_by_name(row, "b_user_id");

            const char *a_id_str;
            size_t a_id_length;
            if (cass_value_get_string(a_id, &a_id_str, &a_id_length) ==
                CASS_OK) {
                std::string a_id_value(a_id_str, a_id_length);
                std::cout << "a_user_id: " << a_id_value << std::endl;
                if (usser_id != a_id_value)
                    contacts[a_id_value] = 'a';
            }

            const char *b_id_str;
            size_t b_id_length;
            if (cass_value_get_string(b_id, &b_id_str, &b_id_length) ==
                CASS_OK) {
                std::string b_id_value(b_id_str, b_id_length);
                std::cout << "b_user_id: " << b_id_value << std::endl;
                if (usser_id != b_id_value)
                    contacts[b_id_value] = 'b';
            }
        }
        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_future_free(result_future);
        cass_statement_free(statement);

        std::ostringstream oss;
        for (const auto &pair : contacts) {
            if (oss.tellp() != 0)
                oss << ",";
            oss << pair.first;
        }
        std::string idList = oss.str();

        std::cout << "List [" << idList << "]\n";

        std::string query =
            "SELECT user_name FROM lunnaria_service.UsersByID WHERE id IN (" +
            idList + ")";

        statement = cass_statement_new(query.c_str(), 0);

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

        while (cass_iterator_next(iterator)) {
            row = cass_iterator_get_row(iterator);
            const CassValue *name_value =
                cass_row_get_column_by_name(row, "user_name");

            std::string user_name;
            const char *name;
            size_t name_length;
            if (cass_value_get_string(name_value, &name, &name_length) ==
                CASS_OK) {
                std::cout << "UX NAME -> " << name << '\n';
                user_name.assign(name, name_length);
                v_user_names.push_back(user_name);
            }
        }

        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_future_free(result_future);
        cass_statement_free(statement);

        std::ostringstream xx;
        for (const auto &pair : v_user_names) {
            if (xx.tellp() != 0)
                xx << ",";
            xx << '\'' << pair << '\'';
        }
        std::string nameList = xx.str();

        std::cout << "NX List -> " << nameList << '\n';

        query = "SELECT * FROM lunnaria_service.Users WHERE user_name IN (" +
                nameList + ")";

        statement = cass_statement_new(query.c_str(), 0);

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

        users = google::protobuf::Arena::Create<ArenaVector<User>>(&arena);

        while (cass_iterator_next(iterator)) {
            row = cass_iterator_get_row(iterator);

            const CassValue *id_value =
                cass_row_get_column_by_name(row, "id");
            const CassValue *name_value =
                cass_row_get_column_by_name(row, "user_name");
            const CassValue *display_name_value =
                cass_row_get_column_by_name(row, "display_name");
            const CassValue *user_email_value =
                cass_row_get_column_by_name(row, "user_email");
            const CassValue *user_avatar_value =
                cass_row_get_column_by_name(row, "user_avatar");
            const CassValue *online_status_value =
                cass_row_get_column_by_name(row, "online_status");
            const CassValue *joined_at_value =
                cass_row_get_column_by_name(row, "joined_at");
            const CassValue *last_activity_value =
                cass_row_get_column_by_name(row, "last_activity");

            std::string user_name, display_name, user_email, user_avatar;
            int32_t online_status;
            int64_t last_activity;

            auto get_string_value =
                [](const CassValue *cass_value) -> std::string {
                const char *str;
                size_t str_length;
                if (cass_value &&
                    cass_value_get_string(cass_value, &str, &str_length) ==
                        CASS_OK) {
                    return std::string(str, str_length);
                }
                return "";
            };

            auto get_int64_value = [](const CassValue *cass_value) -> int64_t {
                cass_int64_t value;
                if (cass_value &&
                    cass_value_get_int64(cass_value, &value) == CASS_OK) {
                    return value;
                }
                return 0;
            };

            auto get_int32_value = [](const CassValue *cass_value) -> int32_t {
                cass_int32_t value;
                if (cass_value &&
                    cass_value_get_int32(cass_value, &value) == CASS_OK) {
                    return value;
                }
                return 0;
            };

            CassUuid user_id_uuid;
            cass_value_get_uuid(id_value, &user_id_uuid);

            char user_id_str[CASS_UUID_STRING_LENGTH];
            cass_uuid_string(user_id_uuid, user_id_str);

            user_name = get_string_value(name_value);
            display_name = get_string_value(display_name_value);
            user_email = get_string_value(user_email_value);
            user_avatar = get_string_value(user_avatar_value);
            online_status = get_int32_value(online_status_value);
            last_activity = get_int64_value(last_activity_value);

            std::cout << "hdisda::: "<< user_id_str << "\n";

            User *user = google::protobuf::Arena::Create<User>(&arena);
            user->set_user_id(user_id_str);
            user->set_user_name(user_name);
            user->set_display_name(display_name);
            user->set_user_email(user_email);
            user->set_user_avatar(user_avatar);
            user->set_online_status(online_status);
            user->set_last_activity(last_activity);

            users->push_back(*user);
        }

        std::cout << "Final  count -> " << users->size() << "\n";

        // Clean up resources
        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_session_free(session);
        cass_cluster_free(cluster);

        return Lxcode::OK(users);
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

Lxcode ScyllaManager::sendMessageToUser(const Message &message, Arena &arena) {

    std::cout << "Sender: " << message.sender_id().c_str() << "\n";
    std::cout << "Receiver: " << message.receiver_id().c_str() << "\n";

    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;
    Message *m;
    std::string receiver_id;

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

        // Query to check if the receiver exists and get their ID
        statement = cass_statement_new(
            "SELECT id FROM lunnaria_service.Users WHERE user_name = ?", 1);
        cass_statement_bind_string(statement, 0, message.receiver_id().c_str());

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing receiver query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute receiver query");
        }

        result = cass_future_get_result(result_future);
        if (cass_result_row_count(result) == 0) {
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_result_free(result);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Receiver not found");
        }

        CassUuid id;
        char id_str[CASS_UUID_STRING_LENGTH];
        row = cass_result_first_row(result);
        const CassValue *id_value = cass_row_get_column_by_name(row, "id");

        if (cass_value_get_uuid(id_value, &id) == CASS_OK) {
            cass_uuid_string(id, id_str);
            receiver_id = std::string(id_str);
            std::cout << "RECEIVER ID: " << receiver_id << std::endl;
        } else {
            std::cerr << "Failed to get UUID from result row" << std::endl;
            // Handle this error appropriately
        }

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_result_free(result);

        // Insert the message
        statement = cass_statement_new(
            "INSERT INTO lunnaria_service.Messages (id, sender_id, "
            "receiver_id, text_content, byte_content, created_at) VALUES "
            "(uuid(), ?, ?, ?, ?, ?)",
            5);

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        int64_t millis_since_epoch = static_cast<int64_t>(timestamp) * 1000;

        cass_statement_bind_string(statement, 0, message.sender_id().c_str());
        cass_statement_bind_string(statement, 1, receiver_id.c_str());
        cass_statement_bind_string(statement, 2,
                                   message.text_content().c_str());
        cass_statement_bind_bytes(statement, 3,
                                  reinterpret_cast<const cass_byte_t *>(
                                      message.byte_content().data()),
                                  message.byte_content().size());
        cass_statement_bind_int64(statement, 4, millis_since_epoch);

        result_future = cass_session_execute(session, statement);
        if (cass_future_error_code(result_future) != CASS_OK) {
            const char *error_message;
            size_t error_message_length;
            cass_future_error_message(result_future, &error_message,
                                      &error_message_length);
            std::cerr << "Error executing insert query: "
                      << std::string(error_message, error_message_length)
                      << std::endl;
            cass_future_free(result_future);
            cass_statement_free(statement);
            cass_session_free(session);
            cass_cluster_free(cluster);
            return Lxcode::DB_ERROR(DB_ERROR_QUERY_FAILED,
                                    "Failed to execute insert query");
        }

        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_session_free(session);
        cass_cluster_free(cluster);

        m = google::protobuf::Arena::Create<Message>(&arena, message);
        m->set_created_at(millis_since_epoch);
        m->set_receiver_id(receiver_id);
        return Lxcode::OK(m);
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

bool operator<(const Message& lhs, const Message& rhs) {
    return lhs.created_at() < rhs.created_at();
}

bool operator>(const Message& lhs, const Message& rhs) {
    return lhs.created_at() > rhs.created_at();
}

Lxcode ScyllaManager::getMessages(const char *sender_id, const char *receiver_id, Arena &arena) {
    std::cout << "Requester: " << sender_id << "\n";
    std::cout << "Receiver Name: " << receiver_id << "\n";

    CassSession *session = cass_session_new();
    CassCluster *cluster = cass_cluster_new();
    CassFuture *connect_future = nullptr;
    CassStatement *statement = nullptr;
    CassFuture *result_future = nullptr;
    const CassResult *result = nullptr;
    CassIterator *iterator = nullptr;
    const CassRow *row = nullptr;
    ArenaSet<Message> *messages = nullptr;

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
            "SELECT * FROM lunnaria_service.Messages WHERE sender_id = ? AND receiver_id = ?", 2);
        cass_statement_bind_string(statement, 0, sender_id);
        cass_statement_bind_string(statement, 1, receiver_id);

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

        while (cass_iterator_next(iterator)) {
            if(!messages)
                messages = google::protobuf::Arena::Create<ArenaSet<Message>>(&arena);

            row = cass_iterator_get_row(iterator);

            Message *msg = google::protobuf::Arena::Create<Message>(&arena);

            // Fetch and set fields for the message
            const CassValue *value;

            // sender_id
            value = cass_row_get_column_by_name(row, "sender_id");
            const char *sender_id_data;
            size_t sender_id_length;
            if (cass_value_get_string(value, &sender_id_data, &sender_id_length) == CASS_OK) {
                msg->set_sender_id(std::string(sender_id_data, sender_id_length));
            }

            // receiver_id
            value = cass_row_get_column_by_name(row, "receiver_id");
            const char *receiver_id_data;
            size_t receiver_id_length;
            if (cass_value_get_string(value, &receiver_id_data, &receiver_id_length) == CASS_OK) {
                msg->set_receiver_id(std::string(receiver_id_data, receiver_id_length));
            }

            // text_content
            value = cass_row_get_column_by_name(row, "text_content");
            const char *text_content;
            size_t text_content_length;
            if (cass_value_get_string(value, &text_content, &text_content_length) == CASS_OK) {
                msg->set_text_content(std::string(text_content, text_content_length));
            }

            // byte_content
            value = cass_row_get_column_by_name(row, "byte_content");
            const cass_byte_t *byte_content_data;
            size_t byte_content_length;
            if (cass_value_get_bytes(value, &byte_content_data, &byte_content_length) == CASS_OK) {
                size_t part_size = 1024; // Example part size, adjust as needed
                for (size_t offset = 0; offset < byte_content_length; offset += part_size) {
                    size_t length = std::min(part_size, byte_content_length - offset);
                    msg->add_byte_content(byte_content_data + offset, length);
                }
            }

            // created_at
            cass_int64_t created_at;
            value = cass_row_get_column_by_name(row, "created_at");
            if (cass_value_get_int64(value, &created_at) == CASS_OK) {
                msg->set_created_at(created_at);
            }

            messages->insert(*msg);
        }

        cass_result_free(result);
        cass_iterator_free(iterator);
        cass_future_free(result_future);
        cass_statement_free(statement);
        cass_session_free(session);
        cass_cluster_free(cluster);

        std::cout << "\n\nFINAL: " << messages->size() << "\n\n";


        return Lxcode::OK(messages);
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

        std::cerr << e.what() << "\n";
        return Lxcode::DB_ERROR(DB_ERROR_STD_EXCEPTION, e.what());
    }
}
