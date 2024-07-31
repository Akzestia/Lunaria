#include "PeerHandler.h"
#include "../../route-manager/Routes.hpp"
#include "../../server/QuicServer.h"

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

// TODO proper handling
void PeerHandler::HandlePeer(HQUIC Stream, const uint8_t &data,
                             size_t dataSize) {
    SetPeer(Stream, data, dataSize);
}

//

bool PeerHandler::onPeerShutdown(HQUIC Stream, void *context) {

    uint8_t *data = (*peers)[Stream];
    size_t dataSize = (*peerDataSizes)[Stream];
    // absl::string_view dataView(reinterpret_cast<const char *>(data),
    // dataSize); absl::Cord receivedCord(dataView);

    std::unique_ptr<Wrapper> wrapper = std::make_unique<Wrapper>();
    if (!wrapper->ParseFromArray(data, dataSize)) {
        std::cerr << "Error: Failed to parse the Cord into a Wrapper"
                  << std::endl;
        return false;
    }

    std::cout << "\nRoute: " << wrapper->route() << "\n";

    switch (wrapper->route()) {
    case SIGN_UP: {
        Lxcode response = RouteManager::handleSignUp(wrapper->auth().sign_up());
        std::unique_ptr<Wrapper> responseWrapper = std::make_unique<Wrapper>();

        responseWrapper->set_route(AUTH_RESPONSE);

        AuthResponse authResponse;
        if (response == Lxcode::OK()) {
            std::cout << "Sign up successful\n";

            authResponse.set_is_successful(true);
            authResponse.set_token(response.response);
            authResponse.set_allocated_user(std::get<User *>(response.payload));

            *responseWrapper->mutable_authresponse() = authResponse;

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *responseWrapper);

            delete std::get<User *>(response.payload);
            return true;
        }

        std::cout << "Sign up failed\n";

        authResponse.set_is_successful(false);

        *responseWrapper->mutable_authresponse() = authResponse;

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
                                                              *responseWrapper);

        return false;
    }
    case SIGN_IN: {
        Lxcode response = RouteManager::handleSignIn(wrapper->auth().sign_in());

        std::unique_ptr<Wrapper> responseWrapper = std::make_unique<Wrapper>();

        responseWrapper->set_route(AUTH_RESPONSE);

        AuthResponse authResponse;
        if (response == Lxcode::OK()) {

            std::cout << "Sign in successful\n";

            authResponse.set_is_successful(true);
            authResponse.set_token(response.response);
            authResponse.set_allocated_user(std::get<User *>(response.payload));

            *responseWrapper->mutable_authresponse() = authResponse;

            reinterpret_cast<QuicServer *>(context)->SendResponse(
                Stream, *responseWrapper);

            delete std::get<User *>(response.payload);
            return true;
        }

        std::cout << "Sign in failed\n";

        authResponse.set_is_successful(false);

        *responseWrapper->mutable_authresponse() = authResponse;

        reinterpret_cast<QuicServer *>(context)->SendResponse(Stream,
                                                              *responseWrapper);

        return false;
    }
    case SEND_FRIEND_REQUEST: {

        break;
    }
    case REMOVE_FRIEND: {

        break;
    }
    case SEND_MESSAGE: {

        break;
    }
    case RECEIVE_MESSAGE: {

        break;
    }
    case CREATE_CHAT_GROUP: {

        break;
    }
    case JOIN_CHAT_GROUP: {

        break;
    }
    case LEAVE_CHAT_GROUP: {

        break;
    }
    case ADD_GROUP_MEMBER: {

        break;
    }
    case REMOVE_GROUP_MEMBER: {

        break;
    }
    case TYPING_INDICATOR: {

        break;
    }
    case READ_RECEIPT: {

        break;
    }
    case DELETE_MESSAGE: {

        break;
    }
    case EDIT_MESSAGE: {

        break;
    }
    case FETCH_CHAT_HISTORY: {

        break;
    }
    case USER_ONLINE_STATUS: {

        break;
    }
    case USER_OFFLINE_STATUS: {

        break;
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
