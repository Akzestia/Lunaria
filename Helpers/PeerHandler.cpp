#include "PeerHandler.h"
#include <cstdint>
#include <iterator>

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

bool PeerHandler::onPeerShutdown(HQUIC Stream) {
  uint8_t *data = (*peers)[Stream];
  size_t dataSize = (*peerDataSizes)[Stream];
  absl::string_view dataView(reinterpret_cast<const char *>(data), dataSize);
  absl::Cord receivedCord(dataView);

  Person *p = new Person();

  if (!p->ParsePartialFromCord(receivedCord)) {
    std::cerr << "Error: Failed to deserialize Person from Cord." << std::endl;
    delete p;
    delete[] data;
    return false;
  } else {
    std::cout << "ID: " << p->id() << std::endl;
    std::cout << "Email: " << p->email() << std::endl;
    std::cout << "\nMessage size:  " << dataSize / 1024 / 1024 << "mb"
              << std::endl;
    delete p;
    delete[] data;
    return true;
  }
}