#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H
#include "../../MsQuic/Linux_x64/include/msquic.h"
#include "../../tokio-cpp/ThreadPool.h"
#include "../clientPeerHandler/ClientPeerHandler.h"
#include "../../error-manager/ErrorManager.h"
#include "../../proto/build/wrapper.pb.h"
#include <atomic>
#include <cstdio>
#include <iostream>
#include <thread>

class ClientListener : protected ClientPeerHandler {
  public:
    ClientListener(const QUIC_API_TABLE *MsQuic, HQUIC Registration,
                   const QUIC_BUFFER Alpn, uint16_t UdpPort, const size_t ThreadNumber, const char *cert,
                   const char *key);
    virtual ~ClientListener();
    ClientListener() = delete;

    void Start();
    void Close();

  private:
    friend class ClientPeerHandler;

    Lxcode SendResponseToServer(HQUIC Stream, const Wrapper &wrapper);

    std::atomic<bool> isRunning = false;
    std::thread serverThread;
    using ClientPeerHandler::HandlePeer;
    using ClientPeerHandler::onPeerShutdown;

    const QUIC_API_TABLE *MsQuic = nullptr;
    HQUIC Listener = nullptr;
    HQUIC Registration = nullptr;
    HQUIC Configuration = nullptr;
    const QUIC_BUFFER Alpn;
    uint16_t UdpPort;

    ThreadPool threadPool;
    ClientPeerHandler clientPeerHandler;

    QUIC_STATUS Status;

    void LoadConfiguration(const char *cert, const char *key);

    _IRQL_requires_max_(PASSIVE_LEVEL)
        _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
        ListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                         _Inout_ QUIC_LISTENER_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                           _Inout_ QUIC_LISTENER_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
        ConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                           _Inout_ QUIC_CONNECTION_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                             _Inout_ QUIC_CONNECTION_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
        StreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                       _Inout_ QUIC_STREAM_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                         _Inout_ QUIC_STREAM_EVENT *Event);
};

#endif
