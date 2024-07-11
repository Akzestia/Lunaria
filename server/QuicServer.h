

#ifndef QUIC_SERVER_H
#define QUIC_SERVER_H

#include "../Helpers/ConnectionManager/ConnectionManager.h"
#include "../Helpers/PeerHandler/PeerHandler.h"
#include "../MsQuic/Linux_x64/include/msquic.h"
#include "../db/DbManager.h"
#include "../proto/build/user.pb.h"
#include <absl/strings/cord.h>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <sys/stat.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include "../tokio-cpp/ThreadPool.h"

class QuicServer : protected ConnectionManager,
                   protected PeerHandler {
  public:
    void Start();

    void Close();

    bool getIsRunning();

    QuicServer(const char *Host, const uint16_t UdpPort, const size_t ThreadNumber, const char* Alpn, const char *cert,
               const char *key = nullptr);

    ~QuicServer();

  private:
    using ConnectionManager::addUser;
    using ConnectionManager::removeUser;
    using DbManager::getContacts;
    using DbManager::getGraphs;
    using DbManager::getMessages;
    using DbManager::getUser;
    using DbManager::test;
    using PeerHandler::GetPeers;
    using PeerHandler::HandlePeer;
    using PeerHandler::onPeerShutdown;
    using PeerHandler::SetPeer;

    const QUIC_API_TABLE *MsQuic = nullptr;
    static std::condition_variable cv;
    static std::condition_variable server_status;
    static std::mutex cv_m;
    static std::mutex server_status_m;
    static bool disconnected;

    static bool getUserCreds(HQUIC, void*);

    bool getUserCreds(HQUIC);

    static void send(HQUIC, void *);

    void send(HQUIC);

    void ServerLoadConfiguration(const char *hash);

    void ServerLoadConfiguration(const char *cert, const char *key);

    _IRQL_requires_max_(PASSIVE_LEVEL)
        _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
        ServerListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                               _Inout_ QUIC_LISTENER_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticServerListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                                 _Inout_ QUIC_LISTENER_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
        ServerConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                                 _Inout_ QUIC_CONNECTION_EVENT *Event);

    static QUIC_STATUS QUIC_API StaticServerConnectionCallback(
        _In_ HQUIC Connection, _In_opt_ void *Context,
        _Inout_ QUIC_CONNECTION_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
        ClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                             _Inout_ QUIC_STREAM_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                               _Inout_ QUIC_STREAM_EVENT *Event);

    QUIC_CREDENTIAL_CONFIG CredConfig;
    QUIC_ADDR Address = {0};
    uint16_t UdpPort;
    HQUIC Connection = nullptr;
    HQUIC Registration = nullptr;
    HQUIC Configuration = nullptr;
    HQUIC Listener = nullptr;
    QUIC_STATUS Status;
    char *Host = nullptr;
    char *cert = nullptr;
    char *key = nullptr;
    const QUIC_REGISTRATION_CONFIG RegConfig = {
        "Server", QUIC_EXECUTION_PROFILE_TYPE_REAL_TIME};
    const QUIC_BUFFER Alpn; // wq-vvv-01 
    std::atomic<bool> isRunning = false;
    std::thread serverThread;

    uint32_t DecodeHexBuffer(_In_z_ const char *HexBuffer,
                             _In_ uint32_t OutBufferLen,
                             _Out_writes_to_(OutBufferLen, return)
                                 uint8_t *OutBuffer);

    uint8_t DecodeHexChar(_In_ char c);

    ThreadPool threadPool;

    QuicServer();
};

#endif