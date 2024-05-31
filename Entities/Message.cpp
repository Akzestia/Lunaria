#include "Message.h"
#include <chrono>
#include <iostream>

std::vector<uint8_t> Message::Serialize() const {
    nlohmann::json j;
    j["message_id"] = message_id;
    j["message_text"] = message_text;
    j["sender_id"] = sender_id;
    j["receiver_id"] = receiver_id;
    j["message_byte_content"] = message_byte_content;
    j["slack_id"] = slack_id;

    std::string jsonString = j.dump();
    return std::vector<uint8_t>(jsonString.begin(), jsonString.end());
}

Message Message::Deserialize(const std::vector<uint8_t>& message) {
    auto start = std::chrono::high_resolution_clock::now();

    std::string jsonString;
    {
        auto startConversion = std::chrono::high_resolution_clock::now();
        jsonString = std::string(message.begin(), message.end());
        auto endConversion = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedConversion = endConversion - startConversion;
        std::cout << "Time taken for conversion to string: " << elapsedConversion.count() << " seconds\n";
    }

    nlohmann::json j = nlohmann::json::parse(jsonString);

    Message m;
    m.message_id = j["message_id"];
    m.message_text = j["message_text"];
    m.sender_id = j["sender_id"];
    m.receiver_id = j["receiver_id"];
    m.message_byte_content = j["message_byte_content"].get<std::vector<uint8_t>>();
    m.slack_id = j["slack_id"];

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    double sizeInMB = static_cast<double>(message.size()) / (1024 * 1024);

    std::cout << "Time taken for deserialization: " << elapsed.count() << " seconds\n";
    std::cout << "Size of received vector: " << sizeInMB << " MB\n";

    return m;
}