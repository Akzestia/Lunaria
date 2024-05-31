#include "User.h"

std::vector<uint8_t> User::Serialize() const {
    nlohmann::json j;
    j["user_id"] = user_id;
    j["user_name"] = user_name;
    j["user_email"] = user_email;
    j["user_password"] = user_password;
    j["user_avatar"] = user_avatar;
    j["slack_id"] = slack_id;

    std::string jsonString = j.dump();
    return std::vector<uint8_t>(jsonString.begin(), jsonString.end());
}

User User::Deserialize(const std::vector<uint8_t>& user) {
    std::string jsonString(user.begin(), user.end());

    nlohmann::json j = nlohmann::json::parse(jsonString);

    User u;
    u.user_id = j["user_id"];
    u.user_name = j["user_name"];
    u.user_email = j["user_email"];
    u.user_password = j["user_password"];
    u.user_avatar = j["user_avatar"].get<std::vector<uint8_t>>();
    u.slack_id = j["slack_id"];

    return u;
}