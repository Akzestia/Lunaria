#ifndef QUICRESPONSE_H
#define QUICRESPONSE_H

#include "../proto/build/authResponse.pb.h"
#include "../proto/build/contact.pb.h"
#include "../proto/build/message.pb.h"
#include <set>
#include <variant>

using QuicResponsePayload = std::variant<AuthResponse *, std::set<Message *>,
                                         std::set<Contact *>, Contact *>;

struct QuicResponse {
    bool success;
    QuicResponsePayload payload;

    template <typename Container> void cleanupContainer(Container *container) {
        for (auto &item : *container)
            delete item;
        delete container;
    }

    void cleanup() {
        std::visit(
            [this](auto ptr) {
                using PtrType = std::decay_t<decltype(ptr)>;

                if constexpr (std::is_pointer_v<PtrType>)
                    delete ptr;
                else if constexpr (std::is_same_v<PtrType,
                                                  std::set<User *> *> ||
                                   std::is_same_v<PtrType,
                                                  std::set<Message *> *>) {
                    cleanupContainer(ptr);
                }
            },
            payload);
    }

    ~QuicResponse() { cleanup(); }
};

#endif // QUICRESPONSE_H
