#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H
#include <cstdint>
#include <variant>
#include "../proto/build/user.pb.h"
#include "../proto/build/contact.pb.h"

typedef struct Lxcode{
  bool is_successful;
  uint8_t error_code;
  const char* response;
  std::variant<User, Contact>* payload;
} Lxcode;

enum Lxcodes : uint8_t {
    //0x00 Success
    SUCCESS = 0x00,

    //0x01 -> 0x10 Auth errors
    AUTH_ERROR_INCORRECT_PAYLOAD_FORMAT = 0x01,
    AUTH_ERROR_INCORRECT_USER_PASSWORD = 0x02,

    //0x31 -> 0x40 Db errors
    DB_ERROR_STD_EXCEPTION = 0x31,
    DB_ERROR_USER_NOT_FOUND = 0x32,
    DB_ERROR_FAILED_TO_ADD_USER = 0x33, 
    DB_ERROR_CONNECTION_FAILED = 0x34,
};


class ErrorManager{
    public:
      operator bool() const;
};

#endif