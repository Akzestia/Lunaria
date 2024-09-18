#ifndef QUICRESPONSE_H
#define QUICRESPONSE_H

#include "../Helpers/ArenaMemoryResource/ArenaMemoryResource.hpp"
#include "../proto/build/authResponse.pb.h"
#include "../proto/build/contact.pb.h"
#include "../proto/build/message.pb.h"
#include <google/protobuf/arena.h>
#include <optional>
#include <variant>

using QuicResponsePayload =
    std::variant<AuthResponse *, ArenaSet<Message> *, ArenaSet<Contact> *,
                 ArenaSet<User> *, ArenaVector<User> *, Contact *, Message *>;

struct QuicResponse {
    bool success;
    QuicResponsePayload payload;
    google::protobuf::Arena *arena;

    QuicResponse() : success(false), payload(), arena(nullptr) {}

    explicit QuicResponse(bool success,
                          google::protobuf::Arena *arena = nullptr)
        : success(success), payload(), arena(arena) {}

    QuicResponse(const QuicResponse &other)
        : success(other.success), arena(other.arena) {
        payload = other.payload;
    }

    QuicResponse(QuicResponse &&other) noexcept
        : success(other.success), payload(std::move(other.payload)),
          arena(other.arena) {
        other.arena = nullptr;
    }

    QuicResponse &operator=(const QuicResponse &other) {
        if (this != &other) {
            success = other.success;
            payload = other.payload;
            arena = other.arena;
        }
        return *this;
    }

    QuicResponse &operator=(QuicResponse &&other) noexcept {
        if (this != &other) {
            success = other.success;
            payload = std::move(other.payload);
            arena = other.arena;
            other.arena = nullptr;
        }
        return *this;
    }

    template <typename T> void set_payload(T *message) { payload = message; }

    template <typename T> std::optional<T *> get_payload() const {
        if (auto ptr = std::get_if<T *>(&payload)) {
            return *ptr;
        }
        return std::nullopt;
    }

    template <typename T> std::optional<T *> extract_payload() {
        return get_payload<T>();
    }

    ~QuicResponse() = default;
};

inline const QuicResponse defaultQuicResponse{false};

#endif // QUICRESPONSE_H
