#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H
#include "../../MsQuic/Linux_x64/include/msquic.h"
#include <atomic>
#include <cstdio>
#include <iostream>
#include <thread>

class ClientListener {
  public:
    ClientListener(const QUIC_API_TABLE *MsQuic);
    ~ClientListener();
    ClientListener() = default;

    void Start(HQUIC Registration, uint16_t UdpPort, const QUIC_BUFFER &Alpn);
    void Close();

  private:
    std::atomic<bool> isRunning = false;
    std::thread serverThread;

    const QUIC_API_TABLE *MsQuic = nullptr;
    HQUIC Listener = nullptr;
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