#include "ClientPeerHandler.h"
#include "../../route-manager/Routes.hpp"

std::unordered_map<HQUIC, uint8_t *> *ClientPeerHandler::peers =
    new std::unordered_map<HQUIC, uint8_t *>();
std::unordered_map<HQUIC, size_t> *ClientPeerHandler::peerDataSizes =
    new std::unordered_map<HQUIC, size_t>();

ClientPeerHandler::~ClientPeerHandler() {
    std::cout << "\nPeers ~\n";
    if (peers)
        delete peers;
    if (peerDataSizes)
        delete peerDataSizes;
}

std::unordered_map<HQUIC, uint8_t *> *ClientPeerHandler::GetPeers() { return peers; }

void ClientPeerHandler::SetPeer(HQUIC Stream, const uint8_t &data, size_t dataSize) {
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
        case AUTH_RESPONSE:{

        }
        break;
        case SERVER_BINDING_REQUEST:{

        }
        break;
    }

    return true;
}

// TODO proper handling
void ClientPeerHandler::HandlePeer(HQUIC Stream, const uint8_t &data,
                             size_t dataSize) {
    SetPeer(Stream, data, dataSize);
}
