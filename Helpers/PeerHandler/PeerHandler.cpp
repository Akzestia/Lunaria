#include "PeerHandler.h"
#include "../../route-manager/Routes.hpp"
#include "../../server/QuicServer.h"
#include <iostream>

std::unordered_map<HQUIC, uint8_t *> *PeerHandler::peers =
    new std::unordered_map<HQUIC, uint8_t *>();
std::unordered_map<HQUIC, size_t> *PeerHandler::peerDataSizes =
    new std::unordered_map<HQUIC, size_t>();

std::unordered_map<HQUIC, uint8_t *> *PeerHandler::GetPeers() { return peers; }

void PeerHandler::SetPeer(HQUIC Stream, const uint8_t &data, size_t dataSize) {
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

void PeerHandler::HandlePeer(HQUIC Stream, const uint8_t &data,
                             size_t dataSize) {
    SetPeer(Stream, data, dataSize);
}

bool PeerHandler::onPeerShutdown(HQUIC Stream, void *context) {

    uint8_t *data = (*peers)[Stream];
    size_t dataSize = (*peerDataSizes)[Stream];
    std::unique_ptr<Wrapper> wrapper = std::make_unique<Wrapper>();
    if (!wrapper->ParseFromArray(data, dataSize)) {
        std::cerr << "Error: Failed to parse the Cord into a Wrapper"
                  << std::endl;
        return false;
    }

    delete [] data;
    peers->erase(Stream);
    peerDataSizes->erase(Stream);

    std::cout << "\nRoute: " << wrapper->route() << "\n";

    switch (wrapper->route()) {
    case SIGN_UP: {
        Lxcode response = RouteManager::handleSignUp(wrapper->auth().sign_up());

        Response rpc_response;
        if (response == Lxcode::OK()) {
            std::cout << "Sign up successful\n";
            std::unique_ptr<Wrapper> responseWrapper = std::make_unique<Wrapper>();

            responseWrapper->set_route(AUTH_RESPONSE_SIGN_UP);

            AuthResponse authResponse;

            try {
                authResponse.set_is_successful(true);
                authResponse.set_token(response.response);
                *authResponse.mutable_user() =
                    *std::get<User *>(response.payload);

                *responseWrapper->mutable_authresponse() = authResponse;

                rpc_response.set_allocated_result(responseWrapper.release());

                reinterpret_cast<QuicServer *>(context)->SendResponse(
                    Stream, rpc_response);
                delete std::get<User *>(response.payload);
                return true;
            } catch (const std::exception &e) {
                std::cerr << e.what() << '\n';
                delete std::get<User *>(response.payload);
            }
        }

        std::cout << "Sign up failed\n";

        *rpc_response.mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
            rpc_response);

        return false;
    }
    case SIGN_IN: {
        Lxcode response = RouteManager::handleSignIn(wrapper->auth().sign_in());

        Response rpc_response;
        if (response == Lxcode::OK()) {
            AuthResponse authResponse;
            std::unique_ptr<Wrapper> responseWrapper =
                std::make_unique<Wrapper>();

            responseWrapper->set_route(AUTH_RESPONSE_SIGN_IN);

            std::cout << "Sign in successful\n";

            authResponse.set_is_successful(true);
            authResponse.set_token(response.response);

            *authResponse.mutable_user() = *std::get<User *>(response.payload);

            *responseWrapper->mutable_authresponse() = authResponse;

            rpc_response.set_allocated_result(responseWrapper.release());

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, rpc_response);

            delete std::get<User *>(response.payload);
            return true;
        }

        std::cout << "Sign in failed\n";

        *rpc_response.mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(
            Stream, rpc_response);
        return false;
    }
    case CREATE_CONTACT: {
        Lxcode response = RouteManager::createContact(wrapper->contact());
        Response rpc_response;
        if (response == Lxcode::OK()) {
            std::cout << "Contact created\n";

            std::unique_ptr<Wrapper> responseWrapper =
                std::make_unique<Wrapper>();
            responseWrapper->set_route(POST_RESPONSE_CONTACT);
            rpc_response.set_allocated_result(responseWrapper.release());

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, rpc_response);
        }

        *rpc_response.mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(
            Stream, rpc_response);
        std::cout << "Contact creation failed\n";

        return false;
    }
    default:
        std::cerr << "Error: Unknown route" << std::endl;
        return false;
    }
    return true;
}

PeerHandler::~PeerHandler() {
    std::cout << "\nPeers ~\n";
    if (peers)
        delete peers;
    if (peerDataSizes)
        delete peerDataSizes;
}
