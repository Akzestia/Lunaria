#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "json.hpp"
#include "nexus_helpers.h"
class Message : public nexus_base {
public:
    int message_id;
    std::string message_text;
    int sender_id;
    int receiver_id;
    std::vector<uint8_t> message_byte_content;
    std::string slack_id;

    std::vector<uint8_t>  Serialize() const;

    static Message Deserialize(const std::vector<uint8_t>& message);
};