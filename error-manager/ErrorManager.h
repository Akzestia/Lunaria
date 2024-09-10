#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include "../proto/build/authResponse.pb.h"
#include "../proto/build/contact.pb.h"
#include "../proto/build/message.pb.h"
#include "../proto/build/server.pb.h"
#include "../proto/build/user.pb.h"

#include <cstdint>
#include <cstdio>
#include <google/protobuf/arena.h>
#include <memory>
#include <set>
#include <string_view>
#include <variant>

using LxPayload =
    std::variant<User*, Contact*, Server*, Message*, AuthResponse*,
                 std::set<User>*, std::set<Message>*>;

enum class LxcodeType : uint8_t {
    OK = 0x00,
    AUTH_ERROR = 0x01,
    DB_ERROR = 0x02,
    UNKNOWN_ERROR = 0x03
};

struct Lxcode {
    LxcodeType type;
    bool is_successful;
    uint8_t error_code;
    std::string response;
    LxPayload payload;

    Lxcode(LxcodeType type, bool is_successful, uint8_t error_code,
           std::string response, LxPayload payload = {})
        : type(type), is_successful(is_successful), error_code(error_code),
          response(std::move(response)), payload(std::move(payload)) {}

    // OK template for arena-allocated payload
    template <typename T>
    static Lxcode OK(T* payload) {
        return Lxcode(LxcodeType::OK, true, 0x00, "Success", LxPayload(payload));
    }

    static Lxcode OK() { return Lxcode(LxcodeType::OK, true, 0x00, "Success"); }

    template <typename T>
    static Lxcode AUTH_ERROR(google::protobuf::Arena* arena, uint8_t code,
                             const std::string& message, T* payload) {
        return Lxcode(LxcodeType::AUTH_ERROR, false, code, message, LxPayload(payload));
    }

    static Lxcode AUTH_ERROR(uint8_t code, const std::string& message) {
        return Lxcode(LxcodeType::AUTH_ERROR, false, code, message);
    }

    template <typename T>
    static Lxcode DB_ERROR(google::protobuf::Arena* arena, uint8_t code,
                           const std::string& message, T* payload) {
        return Lxcode(LxcodeType::DB_ERROR, false, code, message, LxPayload(payload));
    }

    static Lxcode DB_ERROR(uint8_t code, const std::string& message) {
        return Lxcode(LxcodeType::DB_ERROR, false, code, message);
    }

    template <typename T>
    static Lxcode UNKNOWN_ERROR(google::protobuf::Arena* arena, const std::string& message,
                                T* payload) {
        return Lxcode(LxcodeType::UNKNOWN_ERROR, false, 0xFF, message, LxPayload(payload));
    }

    static Lxcode UNKNOWN_ERROR(const std::string& message) {
        return Lxcode(LxcodeType::UNKNOWN_ERROR, false, 0xFF, message);
    }

    bool operator==(const Lxcode& other) const {
        return type == other.type && is_successful == other.is_successful &&
               error_code == other.error_code;
    }

    bool operator==(LxcodeType other) const { return type == other; }
};

enum Lxcodes : uint8_t {
    // 0x00 Success
    SUCCESS = 0x00,
    // 0x01 -> 0x10 Auth errors
    AUTH_ERROR_INCORRECT_PAYLOAD_FORMAT = 0x01,
    AUTH_ERROR_INCORRECT_USER_PASSWORD = 0x02,
    // 0x31 -> 0x40 Db errors
    DB_ERROR_STD_EXCEPTION = 0x31,
    DB_ERROR_USER_NOT_FOUND = 0x32,
    DB_ERROR_FAILED_TO_ADD_USER = 0x33,
    DB_ERROR_CONNECTION_FAILED = 0x34,
    DB_ERROR_INVALID_INPUT = 0x35,
    DB_ERROR_LOGIN_FAILED = 0x36,
    DB_ERROR_UNIQUE_VIOLATION = 0x37,
    DB_ERROR_QUERY_FAILED = 0x38,
    DB_ERROR_USER_EXISTS = 0x39,
    DB_ERROR_INVALID_PASSWORD = 0x3A,
    DB_ERROR_SIGNUP_FAILED = 0x3B,
};

class ErrorManager {
  public:
    operator bool() const;
};

#endif
