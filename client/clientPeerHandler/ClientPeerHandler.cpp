#include "ClientPeerHandler.h"
#include "../../proto/build/authResponse.pb.h"
#include "../../proto/build/wrapper.pb.h"
#include "../../route-manager/Routes.hpp"
#include "../clientListenerModule/ClientListener.h"
#include <condition_variable>
#include <iostream>
#include <memory>

std::unordered_map<HQUIC, uint8_t *> *ClientPeerHandler::peers =
    new std::unordered_map<HQUIC, uint8_t *>();
std::unordered_map<HQUIC, size_t> *ClientPeerHandler::peerDataSizes =
    new std::unordered_map<HQUIC, size_t>();

std::mutex ClientPeerHandler::loginMutex = {};
std::condition_variable_any ClientPeerHandler::login_Cv = {};

std::mutex ClientPeerHandler::signupMutex = {};
std::condition_variable_any ClientPeerHandler::signup_Cv = {};

QuicResponse ClientPeerHandler::loginResponse = {false, nullptr};
bool ClientPeerHandler::waitingForLogin = false;

QuicResponse ClientPeerHandler::signUpResponse = {false, nullptr};
bool ClientPeerHandler::waitingForSignUp = false;

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

void ClientPeerHandler::ReleaseAuthMutex(std::mutex &lock,
                                         std::condition_variable_any &Cv,
                                         AuthType authType, bool success,
                                         const AuthResponse &authResponse) {
    std::unique_lock<std::mutex> ulock(lock);
    switch (authType) {
    case T_SIGN_IN:
        ClientPeerHandler::loginResponse.success = success;
        if (success) {
            ClientPeerHandler::loginResponse.payload = new AuthResponse(authResponse);
        }
        Cv.notify_one();
        ClientPeerHandler::waitingForLogin = false;
        break;
    case T_SIGN_UP:
        ClientPeerHandler::signUpResponse.success = success;
        if (success) {
            ClientPeerHandler::signUpResponse.payload = new AuthResponse(authResponse);
        }
        Cv.notify_one();
        ClientPeerHandler::waitingForSignUp = false;
        break;
    }
    std::cout << "NotifiedX\n";
}

bool ClientPeerHandler::onPeerShutdown(HQUIC Stream, void *context) {
    uint8_t *data = (*peers)[Stream];
    size_t dataSize = (*peerDataSizes)[Stream];

    std::unique_ptr<Wrapper> wrapper = std::make_unique<Wrapper>();
    if (!wrapper->ParseFromArray(data, dataSize)) {
        std::cerr << "Error: Failed to parse the Cord into a Wrapper"
                  << std::endl;
        return false;
    }

    std::cout << "\nRoute: " << wrapper->route() << "\n";

    switch (wrapper->route()) {
    case AUTH_RESPONSE_SIGN_UP: {
        if (wrapper->authresponse().is_successful()) {
            std::cout << "Auth successful SU\n";

            std::cout << "Auth user: " << wrapper->authresponse().user().user_name() << "\n";
            std::cout << wrapper->authresponse().token() << "\n";
            ReleaseAuthMutex(signupMutex, signup_Cv, T_SIGN_UP, 1,
                             wrapper->authresponse());
            return true;
        }
        std::cout << "Auth failed\n";

        ReleaseAuthMutex(signupMutex, signup_Cv, T_SIGN_UP, 0);
        return false;
    }
    case AUTH_RESPONSE_SIGN_IN: {
        if (wrapper->authresponse().is_successful()) {
            std::cout << "Auth successful SI\n";

            std::cout << wrapper->authresponse().token() << "\n";
            ReleaseAuthMutex(loginMutex, login_Cv, T_SIGN_IN, 1,
                             wrapper->authresponse());
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
