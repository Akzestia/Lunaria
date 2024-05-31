#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "json.hpp"
#include "nexus_helpers.h"

class Contact : public nexus_base {
public:
    int contact_id;
    int first_user;
    int second_user;
    std::string slack_id;

    std::vector<uint8_t>  Serialize() const;

    static Contact Deserialize(const std::vector<uint8_t>& contact);
};