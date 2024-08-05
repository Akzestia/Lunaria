#ifndef QUICRESPONSE_H
#define QUICRESPONSE_H

#include <variant>
#include <set>
#include "../proto/build/authResponse.pb.h"
#include "../proto/build/message.pb.h"
#include "../proto/build/contact.pb.h"

struct QuicResponse {
    bool success;
    std::variant<AuthResponse*, std::set<Message*>, std::vector<Contact*>> payload;
};

#endif // QUICRESPONSE_H
