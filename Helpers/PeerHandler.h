#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <cstring>
#include <unordered_map>
#include "../MsQuic/include/msquic.h"
#include <absl/strings/cord.h>
#include "../proto/test.pb.h"

class PeerHandler {
    static std::unordered_map<HQUIC, uint8_t*>* peers;
    static std::unordered_map<HQUIC, size_t>* peerDataSizes;
public:
    static void HandlePeer(HQUIC Stream, const uint8_t& data, size_t dataSize);
    static std::unordered_map<HQUIC, uint8_t*>* GetPeers();
    static void SetPeer(HQUIC Stream, const uint8_t& data, size_t dataSize);
    static bool onPeerShutdown(HQUIC Stream);
    ~PeerHandler();
};