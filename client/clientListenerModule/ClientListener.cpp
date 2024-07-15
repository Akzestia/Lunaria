#include "ClientListener.h"

ClientListener::ClientListener(const QUIC_API_TABLE *MsQuic) : MsQuic(MsQuic) {
    printf("ClientListener constructor\n");
};


ClientListener::~ClientListener() {
    printf("ClientListener destructor\n");

    if(MsQuic != nullptr) {
        MsQuicClose(MsQuic);
        MsQuic = nullptr;
    }

};

//Write alll the functions here
void ClientListener::LoadConfiguration(const char *cert, const char *key) {
    printf("ClientListener LoadConfiguration\n");
};

_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::ListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                         _Inout_ QUIC_LISTENER_EVENT *Event) {
    printf("ClientListener ListenerCallback\n");
    return QUIC_STATUS_SUCCESS;
};


_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StaticListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                         _Inout_ QUIC_LISTENER_EVENT *Event) {
    return reinterpret_cast<ClientListener*>(Context)->ListenerCallback(Listener, Context, Event);
};

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::ConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                         _Inout_ QUIC_CONNECTION_EVENT *Event) {
    printf("ClientListener ConnectionCallback\n");
    return QUIC_STATUS_SUCCESS;
};

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StaticConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                         _Inout_ QUIC_CONNECTION_EVENT *Event) {
    return reinterpret_cast<ClientListener*>(Context)->ConnectionCallback(Connection, Context, Event);
};

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                         _Inout_ QUIC_STREAM_EVENT *Event) {
    printf("ClientListener StreamCallback\n");
    return QUIC_STATUS_SUCCESS;
};

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StaticStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                         _Inout_ QUIC_STREAM_EVENT *Event) {
    return reinterpret_cast<ClientListener*>(Context)->StreamCallback(Stream, Context, Event);
};

