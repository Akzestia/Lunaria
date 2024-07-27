#ifndef CLIENT_PEER_HANDLER_H
#define CLIENT_PEER_HANDLER_H
#include "../../MsQuic/Linux_x64/include/msquic.h"
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <string>

class ClientPeerHandler {
  protected:
    static void HandlePeer(HQUIC Stream, const uint8_t &data, size_t dataSize);
    static std::unordered_map<HQUIC, uint8_t *> *GetPeers();
    static void SetPeer(HQUIC Stream, const uint8_t &data, size_t dataSize);
    static bool onPeerShutdown(HQUIC Stream, void *context);
    virtual ~ClientPeerHandler();

  private:
    static std::unordered_map<HQUIC, uint8_t *> *peers;
    static std::unordered_map<HQUIC, size_t> *peerDataSizes;
    friend class QuicClient;
};

#endif // !CLIENT_PEER_HANDLER_H