#include "Contact.h"

std::vector<uint8_t> Contact::Serialize() const {
    nlohmann::json j;
    j["contact_id"] = contact_id;
    j["first_user"] = first_user;
    j["second_user"] = second_user;
    j["slack_id"] = slack_id;

    std::string jsonString = j.dump();
    return std::vector<uint8_t>(jsonString.begin(), jsonString.end());
}

Contact Contact::Deserialize(const std::vector<uint8_t>& contact) {
    std::string jsonString(contact.begin(), contact.end());

    nlohmann::json j = nlohmann::json::parse(jsonString);

    Contact c;
    c.contact_id = j["contact_id"];
    c.first_user = j["first_user"];
    c.second_user = j["second_user"];
    c.slack_id = j["slack_id"];

    return c;
}