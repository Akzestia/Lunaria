#ifndef QUICRESPONSE_H
#define QUICRESPONSE_H

#include <variant>
#include <set>
#include "../proto/build/authResponse.pb.h"
#include "../proto/build/message.pb.h"
#include "../proto/build/contact.pb.h"

using QuicResponsePayload = std::variant<AuthResponse*, std::set<Message*>, std::vector<Contact*>, Contact*>;

struct QuicResponse {
    bool success;
    QuicResponsePayload payload;
};

#endif // QUICRESPONSE_H
