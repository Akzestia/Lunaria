#include "ClientListener.h"
#include <cstdio>

ClientListener::ClientListener(const QUIC_API_TABLE *MsQuic) : MsQuic(MsQuic) {
    printf("ClientListener constructor\n");
};

ClientListener::~ClientListener() {
    printf("ClientListener destructor\n");

    if(Listener != nullptr){
        printf("Closing Listener\n");
        MsQuic->ListenerClose(Listener);
    }
};

// Write alll the functions here
void ClientListener::LoadConfiguration(const char *cert, const char *key) {
    printf("ClientListener LoadConfiguration\n");
};

_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::ListenerCallback(_In_ HQUIC Listener,
                                     _In_opt_ void *Context,
                                     _Inout_ QUIC_LISTENER_EVENT *Event) {
    printf("ClientListener ListenerCallback\n");
    return QUIC_STATUS_SUCCESS;
};

_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StaticListenerCallback(_In_ HQUIC Listener,
                                           _In_opt_ void *Context,
                                           _Inout_ QUIC_LISTENER_EVENT *Event) {
    return reinterpret_cast<ClientListener *>(Context)->ListenerCallback(
        Listener, Context, Event);
};

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::ConnectionCallback(_In_ HQUIC Connection,
                                       _In_opt_ void *Context,
                                       _Inout_ QUIC_CONNECTION_EVENT *Event) {
    printf("ClientListener ConnectionCallback\n");
    return QUIC_STATUS_SUCCESS;
};

_IRQL_requires_max_(DISPATCH_LEVEL) _Function_class_(QUIC_CONNECTION_CALLBACK)
    QUIC_STATUS QUIC_API ClientListener::StaticConnectionCallback(
        _In_ HQUIC Connection, _In_opt_ void *Context,
        _Inout_ QUIC_CONNECTION_EVENT *Event) {
    return reinterpret_cast<ClientListener *>(Context)->ConnectionCallback(
        Connection, Context, Event);
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
    ClientListener::StaticStreamCallback(_In_ HQUIC Stream,
                                         _In_opt_ void *Context,
                                         _Inout_ QUIC_STREAM_EVENT *Event) {
    return reinterpret_cast<ClientListener *>(Context)->StreamCallback(
        Stream, Context, Event);
};

#pragma region Start()

void ClientListener::Start(HQUIC Registration, uint16_t UdpPort, const QUIC_BUFFER &Alpn) {
    if (!this->isRunning.load()) {
        this->isRunning.store(true);

        QUIC_ADDR Address = {0};
        QuicAddrSetFamily(&Address, QUIC_ADDRESS_FAMILY_UNSPEC);
        QuicAddrSetPort(&Address, UdpPort);

        this->serverThread = std::thread([this, Registration, Alpn, Address]() {
            if (QUIC_FAILED(Status = MsQuic->ListenerOpen(
                                Registration,
                                ClientListener::StaticListenerCallback, this,
                                &Listener))) {
                printf("ListenerOpen failed, 0x%x!\n", Status);
                if (Listener != NULL) {
                    MsQuic->ListenerClose(Listener);
                }
            }

            if (QUIC_FAILED(Status = MsQuic->ListenerStart(Listener, &Alpn, 1,
                                                           &Address))) {
                printf("ListenerStart failed, 0x%x!\n", Status);
                if (Listener != NULL) {
                    MsQuic->ListenerClose(Listener);
                }
            }

            std::cout << "Listener is running..." << std::endl;
        });
    }
}
#pragma endregion

#pragma region Close()
void ClientListener::Close() {
    if (this->isRunning.load()) {
        this->isRunning.store(false);

        if (this->serverThread.joinable()) {
            this->serverThread.join();
            std::cout << "Listener thread has terminated successfully."
                      << std::endl;
        } else {
            std::cout << "Listener thread is not joinable or already joined."
                      << std::endl;
        }
    } else {
        std::cout << "Listener is not running." << std::endl;
    }

    std::cout << "isRunning: " << this->isRunning.load() << std::endl;
}
#pragma endregion
