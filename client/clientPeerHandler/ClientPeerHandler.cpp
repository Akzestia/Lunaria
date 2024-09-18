#include "ClientPeerHandler.h"
#include "../../proto/build/authResponse.pb.h"
#include "../../proto/build/rpc_response.pb.h"
#include "../../route-manager/Routes.hpp"
#include "../clientListenerModule/ClientListener.h"
#include <condition_variable>
#include <cstdio>
#include <google/protobuf/arena.h>
#include <iostream>
#include <memory>

bool operator<(const Message& lhs, const Message& rhs) {
    return lhs.created_at() < rhs.created_at();
}

bool operator>(const Message& lhs, const Message& rhs) {
    return lhs.created_at() > rhs.created_at();
}

std::unordered_map<HQUIC, uint8_t *> *ClientPeerHandler::peers =
    new std::unordered_map<HQUIC, uint8_t *>();
std::unordered_map<HQUIC, size_t> *ClientPeerHandler::peerDataSizes =
    new std::unordered_map<HQUIC, size_t>();

std::mutex ClientPeerHandler::loginMutex = {};
std::condition_variable_any ClientPeerHandler::login_Cv = {};

std::mutex ClientPeerHandler::signupMutex = {};
std::condition_variable_any ClientPeerHandler::signup_Cv = {};

std::mutex ClientPeerHandler::contactMutex = {};
std::condition_variable_any ClientPeerHandler::contact_Cv = {};

std::mutex ClientPeerHandler::messageMutex = {};
std::condition_variable_any ClientPeerHandler::message_Cv = {};

QuicResponse ClientPeerHandler::loginResponse = defaultQuicResponse;
bool ClientPeerHandler::waitingForLogin = false;

QuicResponse ClientPeerHandler::signUpResponse = defaultQuicResponse;
bool ClientPeerHandler::waitingForSignUp = false;

QuicResponse ClientPeerHandler::contactResponse_POST = defaultQuicResponse;
QuicResponse ClientPeerHandler::contactResponse_PUT = defaultQuicResponse;
QuicResponse ClientPeerHandler::contactResponse_DELETE = defaultQuicResponse;
QuicResponse ClientPeerHandler::contactResponse_GET = defaultQuicResponse;

bool ClientPeerHandler::waitingForContact_POST = false;
bool ClientPeerHandler::waitingForContact_PUT = false;
bool ClientPeerHandler::waitingForContact_DELETE = false;
bool ClientPeerHandler::waitingForContact_GET = false;

QuicResponse ClientPeerHandler::serverResponse_POST = defaultQuicResponse;
QuicResponse ClientPeerHandler::serverResponse_PUT = defaultQuicResponse;
QuicResponse ClientPeerHandler::serverResponse_DELETE = defaultQuicResponse;
QuicResponse ClientPeerHandler::serverResponse_GET = defaultQuicResponse;

bool ClientPeerHandler::waitingForServer_POST = false;
bool ClientPeerHandler::waitingForServer_PUT = false;
bool ClientPeerHandler::waitingForServer_DELETE = false;
bool ClientPeerHandler::waitingForServer_GET = false;

QuicResponse ClientPeerHandler::messageResponse_POST = defaultQuicResponse;
QuicResponse ClientPeerHandler::messageResponse_PUT = defaultQuicResponse;
QuicResponse ClientPeerHandler::messageResponse_DELETE = defaultQuicResponse;
QuicResponse ClientPeerHandler::messageResponse_GET = defaultQuicResponse;

bool ClientPeerHandler::waitingForMessage_POST = false;
bool ClientPeerHandler::waitingForMessage_PUT = false;
bool ClientPeerHandler::waitingForMessage_DELETE = false;
bool ClientPeerHandler::waitingForMessage_GET = false;

Arena *ClientPeerHandler::signInArenaRef = nullptr;
Arena *ClientPeerHandler::signUpArenaRef = nullptr;
Arena *ClientPeerHandler::contactPostArenaRef = nullptr;
Arena *ClientPeerHandler::contactGetArenaRef = nullptr;
Arena *ClientPeerHandler::messageGetArenaRef = nullptr;
Arena *ClientPeerHandler::messagePostArenaRef = nullptr;

ClientPeerHandler::~ClientPeerHandler() {
    std::cout << "\nPeers ~\n";
    if (peers)
        delete peers;
    if (peerDataSizes)
        delete peerDataSizes;
}

std::unordered_map<HQUIC, uint8_t *> *ClientPeerHandler::GetPeers() {
    return peers;
}

void ClientPeerHandler::SetPeer(HQUIC Stream, const uint8_t &data,
                                size_t dataSize) {
    if (peers->find(Stream) != peers->end()) {
        size_t currentSize = (*peerDataSizes)[Stream];
        size_t newSize = currentSize + dataSize;
        uint8_t *newData = new uint8_t[newSize];

        std::memcpy(newData, (*peers)[Stream], currentSize);
        std::memcpy(newData + currentSize, &data, dataSize);

        delete[] (*peers)[Stream];

        (*peers)[Stream] = newData;
        (*peerDataSizes)[Stream] = newSize;
    } else {
        uint8_t *newData = new uint8_t[dataSize];
        std::memcpy(newData, &data, dataSize);

        (*peers)[Stream] = newData;
        (*peerDataSizes)[Stream] = dataSize;
    }
}

std::mutex &ClientPeerHandler::GetLoginMutex() { return loginMutex; }

std::condition_variable_any &ClientPeerHandler::GetLoginCv() {
    return login_Cv;
}

std::mutex &ClientPeerHandler::GetSignUpMutex() { return signupMutex; }

std::condition_variable_any &ClientPeerHandler::GetSignUpCv() {
    return signup_Cv;
}

std::mutex &ClientPeerHandler::GetContactMutex() { return contactMutex; }

std::condition_variable_any &ClientPeerHandler::GetContactCv() {
    return contact_Cv;
}

std::mutex &ClientPeerHandler::GetMessageMutex() { return messageMutex; }

std::condition_variable_any &ClientPeerHandler::GetMessageCv() {
    return message_Cv;
}

void ClientPeerHandler::ReleaseAuthMutex(std::mutex &lock,
                                         std::condition_variable_any &Cv,
                                         AuthType authType, bool success,
                                         const AuthResponse &authResponse) {
    std::unique_lock<std::mutex> ulock(lock);
    switch (authType) {
    case T_SIGN_IN:
        ClientPeerHandler::loginResponse.success = success;
        if (success) {
            ClientPeerHandler::loginResponse.payload =
                google::protobuf::Arena::Create<AuthResponse>(signInArenaRef,
                                                              authResponse);
        }
        Cv.notify_one();
        ClientPeerHandler::waitingForLogin = false;
        break;
    case T_SIGN_UP:
        ClientPeerHandler::signUpResponse.success = success;
        if (success) {
            ClientPeerHandler::signUpResponse.payload =
                google::protobuf::Arena::Create<AuthResponse>(signUpArenaRef,
                                                              authResponse);
        }
        Cv.notify_one();
        ClientPeerHandler::waitingForSignUp = false;
        break;
    }
    std::cout << "NotifiedX\n";
}

void ClientPeerHandler::ReleaseAnyMutex(std::mutex &lock,
                                        std::condition_variable_any &Cv,
                                        ReleaseMutexType type, bool success,
                                        const QuicResponse &quicResponse) {
    std::unique_lock<std::mutex> ulock(lock);
    switch (type) {
    case T_CONTACT_POST: {
        if (success) {
            ClientPeerHandler::contactResponse_POST =
                QuicResponse(quicResponse);
        }
        Cv.notify_one();
        ClientPeerHandler::waitingForContact_POST = false;
    } break;
    case T_CONTACT_PUT: {

    } break;
    case T_CONTACT_GET: {
        if (success)
            ClientPeerHandler::contactResponse_GET = QuicResponse(quicResponse);
        Cv.notify_one();
        ClientPeerHandler::waitingForContact_GET = false;
    } break;
    case T_CONTACT_DELETE: {

    } break;
    case T_MESSAGE_POST: {
        if (success)
            ClientPeerHandler::messageResponse_POST =
                QuicResponse(quicResponse);
        Cv.notify_one();
        ClientPeerHandler::waitingForMessage_POST = false;
    } break;
    case T_MESSAGE_GET: {
        if (success)
            ClientPeerHandler::messageResponse_GET =
                QuicResponse(quicResponse);
        Cv.notify_one();
        ClientPeerHandler::waitingForMessage_GET = false;
    } break;
    default: {

    }
    }
    printf("\nNotifiedY\n");
}

void ClientPeerHandler::ReleaseAll() {
    std::unique_lock<std::mutex> loginLock(loginMutex, std::defer_lock);
    std::unique_lock<std::mutex> signupLock(signupMutex, std::defer_lock);
    std::unique_lock<std::mutex> contactLock(contactMutex, std::defer_lock);
    std::unique_lock<std::mutex> messageLock(messageMutex, std::defer_lock);

    std::lock(loginLock, signupLock, contactLock, messageLock);

    waitingForLogin = false;
    waitingForSignUp = false;
    waitingForContact_POST = false;
    waitingForContact_PUT = false;
    waitingForContact_DELETE = false;
    waitingForContact_GET = false;
    waitingForServer_POST = false;
    waitingForServer_PUT = false;
    waitingForServer_DELETE = false;
    waitingForServer_GET = false;
    waitingForMessage_GET = false;
    waitingForMessage_POST = false;

    login_Cv.notify_all();
    signup_Cv.notify_all();
    contact_Cv.notify_all();
    message_Cv.notify_all();
}

bool ClientPeerHandler::onPeerShutdown(HQUIC Stream, void *context) {
    uint8_t *data = (*peers)[Stream];
    size_t dataSize = (*peerDataSizes)[Stream];

    Arena ax;
    Response *response = google::protobuf::Arena::Create<Response>(&ax);
    if (!response->ParseFromArray(data, dataSize)) {
        std::cerr << "Error: Failed to parse the Cord into a response"
                  << std::endl;
        ClientPeerHandler::ReleaseAll();
        return false;
    }

    if (response->has_error() || !response->has_body()) {
        printf("Error code: %d", response->error().code());
        ClientPeerHandler::ReleaseAll();
        return false;
    }

    switch (response->route()) {
    case AUTH_RESPONSE_SIGN_UP: {
        if (response->body().au_response().is_successful()) {
            std::cout << "Auth successful SU\n";

            ReleaseAuthMutex(signupMutex, signup_Cv, T_SIGN_UP, 1,
                             response->body().au_response());
            return true;
        }
        std::cout << "Auth failed\n";

        ReleaseAuthMutex(signupMutex, signup_Cv, T_SIGN_UP, 0);
        return false;
    }
    case AUTH_RESPONSE_SIGN_IN: {
        if (response->body().au_response().is_successful()) {
            std::cout << "Auth successful SI\n";
            ReleaseAuthMutex(loginMutex, login_Cv, T_SIGN_IN, 1,
                             response->body().au_response());
            return true;
        }
        std::cout << "Auth failed\n";

        ReleaseAuthMutex(loginMutex, login_Cv, T_SIGN_IN, 0);
        return false;
    }
    case SERVER_BINDING_REQUEST: {
        std::cout << "Server binding request\n";
        auto listener = reinterpret_cast<ClientListener *>(context);

        return true;
    }
    case POST_RESPONSE_CONTACT: {
        std::cout << "Has ct_response: " << response->body().has_ct_response()
                  << "\n";
        std::cout << "Has rpc_body: " << response->has_body() << "\n";
        if (response->body().has_ct_response()) {
            std::cout << "Contact response\n";
            QuicResponse quicResponse;
            quicResponse.payload = google::protobuf::Arena::Create<Contact>(
                contactPostArenaRef, response->body().ct_response());
            quicResponse.success = true;
            ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_POST, 1,
                            quicResponse);
            return true;
        }
        ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_POST, false);
        printf("Failed to add contact");
        return false;
    }
    case FETCH_CONTACTS_RESPONSE: {
        if (response->body().has_f_contacts_response()) {
            std::cout
                << "FETCH_CONTACTS_RESPONSE: "
                << response->body().f_contacts_response().response().size()
                << '\n';
            QuicResponse quicResponse;
            quicResponse.payload =
                google::protobuf::Arena::Create<ArenaVector<User>>(
                    contactGetArenaRef,
                    response->body().f_contacts_response().response().begin(),
                    response->body().f_contacts_response().response().end());
            quicResponse.success = true;
            ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_GET, true,
                            quicResponse);
            return true;
        }
        ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_GET, false);
        return false;
    }
    case SEND_MESSAGE_TO_USER_RESPONSE: {
        if (response->body().has_send_message_response()) {
            std::cout << "SEND_MESSAGE_TO_USER_RESPONSE: " << '\n';
            QuicResponse quicResponse;

            quicResponse.payload = google::protobuf::Arena::Create<Message>(
                messagePostArenaRef, response->body().send_message_response());
            quicResponse.success = true;

            ReleaseAnyMutex(messageMutex, message_Cv, T_MESSAGE_POST, true,
                            quicResponse);
            return true;
        }
        ReleaseAnyMutex(messageMutex, message_Cv, T_MESSAGE_POST, false);
        return false;
    }
    case FETCH_DM_MESSAGES_RESPONSE: {
        if (response->body().has_f_messages_response()) {
            std::cout << "FETCH_DM_MESSAGES_RESPONSE: " << response->body().f_messages_response().response().size() << '\n';
            QuicResponse quicResponse;

            quicResponse.payload =
                google::protobuf::Arena::Create<ArenaSet<Message>>(
                    messageGetArenaRef,
                    response->body().f_messages_response().response().begin(),
                    response->body().f_messages_response().response().end());
            quicResponse.success = true;

            std::cout << "FETCH_DM_MESSAGES_RESPONSE F: " << std::get<ArenaSet<Message>*>(quicResponse.payload)->size() << '\n';
            ReleaseAnyMutex(messageMutex, message_Cv, T_MESSAGE_GET, true,
                            quicResponse);

            return true;
        }
        ReleaseAnyMutex(messageMutex, message_Cv, T_MESSAGE_GET, false);
        return false;
    }
    default:
        std::cerr << "Error: Unknown route\n";
        return false;
    }
}

void ClientPeerHandler::HandlePeer(HQUIC Stream, const uint8_t &data,
                                   size_t dataSize) {
    SetPeer(Stream, data, dataSize);
}

void ClientPeerHandler::SetSignInArena(Arena *arena) { signInArenaRef = arena; }

void ClientPeerHandler::SetSignUpArena(Arena *arena) { signUpArenaRef = arena; }

void ClientPeerHandler::SetContactPostArena(Arena *arena) {
    contactPostArenaRef = arena;
}

void ClientPeerHandler::SetContactGetArena(Arena *arena) {
    contactGetArenaRef = arena;
}

void ClientPeerHandler::SetMessageGetArena(Arena *arena) {
    messageGetArenaRef = arena;
}
void ClientPeerHandler::SetMessagePostArena(Arena *arena) {
    messagePostArenaRef = arena;
}
