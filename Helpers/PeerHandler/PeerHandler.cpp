#include "PeerHandler.h"
#include "../../proto/build/rpc_body.pb.h"
#include "../../proto/build/rpc_request.pb.h"
#include "../../route-manager/Routes.hpp"
#include "../../server/QuicServer.h"
#include <cstdio>
#include <google/protobuf/arena.h>
#include <google/protobuf/repeated_field.h>
#include <iostream>
#include <memory>
#include <vector>

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

    std::shared_ptr<Request> request = std::make_unique<Request>();
    if (!request->ParseFromArray(data, dataSize)) {
        std::cerr << "Error: Failed to parse the Cord into a request"
                  << std::endl;
        return false;
    }

    delete[] data;
    peers->erase(Stream);
    peerDataSizes->erase(Stream);
    if (request->has_error()) {
        printf("Error proccessign request");
        return false;
    }

    std::cout << "\nRoute: " << request->route() << "\n";
    std::cout << "\nPassword " << request->body().si_request().user_password()
              << "\n";

    switch (request->route()) {
    case SIGN_UP: {
        Arena arena;
        Response *rpc_response =
            google::protobuf::Arena::Create<Response>(&arena);
        if (!request->has_body() || !request->body().has_su_request()) {
            *rpc_response->mutable_error() = Error();

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
            std::cout << "SignInX failed\n";
            return false;
        }

        Lxcode response =
            RouteManager::handleSignUp(request->body().su_request(), arena);

        if (response == Lxcode::OK()) {
            std::cout << "Sign up successful\n";
            rpc_response->set_route(AUTH_RESPONSE_SIGN_UP);
            AuthResponse *authResponse =
                google::protobuf::Arena::Create<AuthResponse>(&arena);

            try {
                authResponse->set_is_successful(true);
                authResponse->set_token(response.response);
                *authResponse->mutable_user() =
                    *std::get<User *>(response.payload);

                Body *rpc_body = google::protobuf::Arena::Create<Body>(&arena);
                *rpc_body->mutable_au_response() = *authResponse;
                *rpc_response->mutable_body() = *rpc_body;

                reinterpret_cast<QuicServer *>(context)->SendResponse(
                    Stream, *rpc_response);
                return true;
            } catch (const std::exception &e) {
                std::cerr << e.what() << '\n';
            }
        }

        std::cout << "Sign up failed\n";

        *rpc_response->mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
                                                              *rpc_response);

        return false;
    }
    case SIGN_IN: {
        Arena arena;
        Response *rpc_response =
            google::protobuf::Arena::Create<Response>(&arena);

        if (!request->has_body() || !request->body().has_si_request()) {
            *rpc_response->mutable_error() = Error();

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
            std::cout << "SignInX failed\n";
            return false;
        }

        printf("SignInX");

        Lxcode response =
            RouteManager::handleSignIn(request->body().si_request(), arena);

        if (response == Lxcode::OK()) {
            AuthResponse *authResponse =
                google::protobuf::Arena::Create<AuthResponse>(&arena);

            rpc_response->set_route(AUTH_RESPONSE_SIGN_IN);

            std::cout << "Sign in successful\n";

            authResponse->set_is_successful(true);
            authResponse->set_token(response.response);

            *authResponse->mutable_user() = *std::get<User *>(response.payload);

            Body *rpc_body = google::protobuf::Arena::Create<Body>(&arena);
            *rpc_body->mutable_au_response() = *authResponse;
            *rpc_response->mutable_body() = *rpc_body;

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
            return true;
        }

        std::cout << "Sign in failed\n";

        *rpc_response->mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
                                                              *rpc_response);
        return false;
    }
    case CREATE_CONTACT: {

        std::cout << "Creation started\n";

        Arena arena;
        Response *rpc_response =
            google::protobuf::Arena::Create<Response>(&arena);

        if (!request->has_body() || !request->body().has_ct_request()) {
            *rpc_response->mutable_error() = Error();

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
            std::cout << "Contact creation failed\n";
            return false;
        }
        std::cout << "Arena started\n";
        Lxcode response =
            RouteManager::createContact(request->body().ct_request(), arena);

        if (response == Lxcode::OK()) {
            std::cout << "Contact created\n";

            rpc_response->set_route(POST_RESPONSE_CONTACT);
            Body rpc_body;
            *rpc_body.mutable_ct_response() =
                *std::get<Contact *>(response.payload);
            *rpc_response->mutable_body() = rpc_body;

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
        }

        *rpc_response->mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
                                                              *rpc_response);
        std::cout << "Contact creation failed\n";

        return false;
    }
    case FETCH_CONTACTS_BY_ID: {

        std::cout << "Fetching contacts...\n";

        Arena arena;
        Response *rpc_response =
            google::protobuf::Arena::Create<Response>(&arena);

        if (!request->has_body() || !request->body().has_f_contacts()) {
            *rpc_response->mutable_error() = Error();

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
            std::cout << "Fetching contacts failed\n";
            return false;
        }

        const char *user_id = request->body().f_contacts().user_id().c_str();
        Lxcode response = RouteManager::getContacts(user_id, arena);

        if (response == Lxcode::OK()) {
            std::cout << "Contacts fetched\n";

            rpc_response->set_route(FETCH_CONTACTS_RESPONSE);
            Body *rpc_body = google::protobuf::Arena::Create<Body>(&arena);
            FetchResponseContacts *f_c =
                google::protobuf::Arena::Create<FetchResponseContacts>(&arena);

            auto* ptr = std::get<ArenaVector<User>*>(response.payload);
            google::protobuf::RepeatedPtrField<User>* field = f_c->mutable_response();
            field->Assign(ptr->begin(), ptr->end());

            *rpc_body->mutable_f_contacts_response() = *f_c;
            *rpc_response->mutable_body() = *rpc_body;

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *rpc_response);
        }

        *rpc_response->mutable_error() = Error();

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
                                                              *rpc_response);
        std::cout << "Fetching contacts failed\n";

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
