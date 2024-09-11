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
#include <set>

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

Arena* ClientPeerHandler::signInArenaRef = nullptr;
Arena* ClientPeerHandler::signUpArenaRef = nullptr;
Arena* ClientPeerHandler::contactPostArenaRef = nullptr;

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
        ClientPeerHandler::contactResponse_POST.success = success;
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
        ClientPeerHandler::contactResponse_GET.success = success;
        if(success)
            ClientPeerHandler::contactResponse_GET = QuicResponse(quicResponse);
    } break;
    case T_CONTACT_DELETE: {

    } break;
    }
    printf("\nNotifiedY\n");
}

bool ClientPeerHandler::onPeerShutdown(HQUIC Stream, void *context) {
    uint8_t *data = (*peers)[Stream];
    size_t dataSize = (*peerDataSizes)[Stream];

    std::unique_ptr<Response> response = std::make_unique<Response>();
    if (!response->ParseFromArray(data, dataSize)) {
        std::cerr << "Error: Failed to parse the Cord into a response"
                  << std::endl;
        return false;
    }

    if (response->has_error() || !response->has_body()) {
        printf("Error code: %d", response->error().code());
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
        if (response->body().has_ct_response()) {
            std::cout << "Contact response\n";
            QuicResponse quicResponse;
            quicResponse.payload = google::protobuf::Arena::Create<Contact>(
                contactPostArenaRef, response->body().ct_response());
            ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_POST, true,
                            quicResponse);
            return true;
        }
        ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_POST, false);
        printf("Failed to add contact");
        return false;
    }
    case FETCH_CONTACTS_RESPONSE: {
        if (response->body().has_f_contacts_response()) {

            QuicResponse quicResponse;
            quicResponse.payload = google::protobuf::Arena::Create<std::set<User>>(contactGetArenaRef, response->body().f_contacts_response());
            ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_GET, true,
                            quicResponse);
            return true;
        }
        ReleaseAnyMutex(contactMutex, contact_Cv, T_CONTACT_GET, false);
        return false;
    }
    default:
        std::cerr << "Error: Unknown route\n";
        return false;
    }
}

// TODO proper handling
void ClientPeerHandler::HandlePeer(HQUIC Stream, const uint8_t &data,
                                   size_t dataSize) {
    SetPeer(Stream, data, dataSize);
}

void ClientPeerHandler::SetSignInArena(Arena *arena){
    signInArenaRef = arena;
}

void ClientPeerHandler::SetSignUpArena(Arena *arena){
    signUpArenaRef = arena;
}

void ClientPeerHandler::SetContactPostArena(Arena *arena){
    contactPostArenaRef = arena;
}
