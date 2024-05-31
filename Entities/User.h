#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "json.hpp"
#include "nexus_helpers.h"

class User : public nexus_base {
public:
    int user_id;
    std::string user_name;
    std::string user_email;
    std::string user_password;
    std::vector<uint8_t> user_avatar;
    std::string slack_id;

    std::vector<uint8_t>  Serialize() const;

    static User Deserialize(const std::vector<uint8_t>& user);
};