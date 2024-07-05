#pragma once
#include "../../MsQuic/include/msquic.h"
#include "../../proto/build/test.pb.h"
#include "../../proto/build/user.pb.h"
#include "../../proto/build/wrapper.pb.h"
#include <absl/strings/cord.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>

//Todo mutex
class PeerHandler {
    PeerHandler();
  private:
    static std::unordered_map<HQUIC, uint8_t *> *peers;
    static std::unordered_map<HQUIC, size_t> *peerDataSizes;
  protected:
    static void HandlePeer(HQUIC Stream, const uint8_t &data, size_t dataSize);
    static std::unordered_map<HQUIC, uint8_t *> *GetPeers();
    static void SetPeer(HQUIC Stream, const uint8_t &data, size_t dataSize);
    static bool onPeerShutdown(HQUIC Stream);
    virtual ~PeerHandler();
};