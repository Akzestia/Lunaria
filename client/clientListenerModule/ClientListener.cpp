#include "ClientListener.h"
#include <cstdio>
#include <iostream>

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (void)(P)
#endif

typedef struct QUIC_CREDENTIAL_CONFIG_HELPER {
    QUIC_CREDENTIAL_CONFIG CredConfig;
    union {
        QUIC_CERTIFICATE_HASH CertHash;
        QUIC_CERTIFICATE_HASH_STORE CertHashStore;
        QUIC_CERTIFICATE_FILE CertFile;
        QUIC_CERTIFICATE_FILE_PROTECTED CertFileProtected;
    };
} QUIC_CREDENTIAL_CONFIG_HELPER;


ClientListener::ClientListener(const QUIC_API_TABLE *MsQuic, HQUIC Registration, const QUIC_BUFFER Alpn, uint16_t UdpPort, const char* cert, const char* key) : MsQuic(MsQuic),
 Registration(Registration),
  Alpn(Alpn), 
  UdpPort(UdpPort) {
    printf("ClientListener constructor\n");
    LoadConfiguration(cert, key);
};

ClientListener::~ClientListener() {
    printf("ClientListener destructor\n");

    if(Listener != nullptr){
        printf("Closing Listener\n");
        MsQuic->ListenerClose(Listener);
    }
};

void ClientListener::LoadConfiguration(const char *cert, const char *key) {
    QUIC_SETTINGS Settings = {0};
    Settings.IdleTimeoutMs = 0;
    Settings.IsSet.IdleTimeoutMs = TRUE;

    Settings.ServerResumptionLevel =
        QUIC_SERVER_RESUME_AND_ZERORTT; // QUIC_SERVER_RESUME_AND_ZERORTT
    Settings.IsSet.ServerResumptionLevel = TRUE;

    Settings.PeerBidiStreamCount = 100;
    Settings.IsSet.PeerBidiStreamCount = TRUE;

    Settings.PeerUnidiStreamCount = 100;
    Settings.IsSet.PeerUnidiStreamCount = TRUE;

#pragma region OpenSslCert
    QUIC_CREDENTIAL_CONFIG_HELPER Config;
    memset(&Config, 0, sizeof(Config));
    Config.CredConfig.Flags =
        QUIC_CREDENTIAL_FLAG_NO_CERTIFICATE_VALIDATION; // QUIC_CREDENTIAL_FLAG_REQUIRE_CLIENT_AUTHENTICATION

    // printf("cert.pem exists: %d\n", fileExists(cert));
    // printf("key.pem exists: %d\n", fileExists(key));

    const char *Cert = cert;
    const char *KeyFile = key;

    Config.CertFile.CertificateFile = (char *)Cert;
    Config.CertFile.PrivateKeyFile = (char *)KeyFile;
    Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
    Config.CredConfig.CertificateFile = &Config.CertFile;
#pragma endregion

    QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
    if (QUIC_FAILED(Status = MsQuic->ConfigurationOpen(
                        Registration, &Alpn, 1, &Settings, sizeof(Settings),
                        this, &Configuration))) {
        printf("ConfigurationOpen failed, 0x%x!\n", Status);
    }

    if (QUIC_FAILED(Status = MsQuic->ConfigurationLoadCredential(
                        Configuration, &Config.CredConfig))) {
        printf("ConfigurationLoadCredential failed, 0x%x!\n", Status);
    }
}


_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::ListenerCallback(_In_ HQUIC Listener,
                                     _In_opt_ void *Context,
                                     _Inout_ QUIC_LISTENER_EVENT *Event) {
    UNREFERENCED_PARAMETER(Listener);
    UNREFERENCED_PARAMETER(Context);
    QUIC_STATUS Status = QUIC_STATUS_NOT_SUPPORTED;
    switch (Event->Type) {
    case QUIC_LISTENER_EVENT_NEW_CONNECTION:
        MsQuic->SetCallbackHandler(
            Event->NEW_CONNECTION.Connection,
            (void *)ClientListener::StaticConnectionCallback, this);
        Status = MsQuic->ConnectionSetConfiguration(
            Event->NEW_CONNECTION.Connection, Configuration);
        break;
    case QUIC_LISTENER_EVENT_STOP_COMPLETE: {

    } break;
    default:
        break;
    }
    return Status;
};

_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StaticListenerCallback(_In_ HQUIC Listener,
                                           _In_opt_ void *Context,
                                           _Inout_ QUIC_LISTENER_EVENT *Event) {
    return reinterpret_cast<ClientListener *>(Context)->ListenerCallback(
        Listener, Context, Event);
};

#pragma region ServerConnectionCallback
_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::ConnectionCallback(_In_ HQUIC Connection,
                                         _In_opt_ void *Context,
                                         _Inout_ QUIC_CONNECTION_EVENT *Event) {
    UNREFERENCED_PARAMETER(Context);
    switch (Event->Type) {
    case QUIC_CONNECTION_EVENT_CONNECTED: {

        printf("[conn][%p] Connected\n", Connection);
        MsQuic->ConnectionSendResumptionTicket(
            Connection, QUIC_SEND_RESUMPTION_FLAG_NONE, 0, NULL);
    } break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
        if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status ==
            QUIC_STATUS_CONNECTION_IDLE) {
            printf("[conn][%p] Successfully shut down on idle.\n", Connection);
        } else {
            printf("[conn][%p] Shut down by transport, 0x%x\n", Connection,
                   Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
        }
        break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER: {
        printf("[conn][%p] Shut down by peer, 0x%llu\n", Connection,
               (unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
    } break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
        printf("[conn][%p] All done\n", Connection);
        MsQuic->ConnectionClose(Connection);
        break;
    case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED: {
        printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
        HQUIC Stream = Event->PEER_STREAM_STARTED.Stream;
        MsQuic->SetCallbackHandler(Stream, (void *)ClientListener::StaticStreamCallback, this);
        break;
    }
    case QUIC_CONNECTION_EVENT_RESUMED:
        printf("[conn][%p] Connection resumed!\n", Connection);
        break;
    case QUIC_CONNECTION_EVENT_IDEAL_PROCESSOR_CHANGED: {

        break;
    }
    default:
        printf("\nDEFAULT: QUIC_EVENT of type %d occurred\n", Event->Type);
        break;
    }
    return QUIC_STATUS_SUCCESS;
}
#pragma endregion

_IRQL_requires_max_(DISPATCH_LEVEL) _Function_class_(QUIC_CONNECTION_CALLBACK)
    QUIC_STATUS QUIC_API ClientListener::StaticConnectionCallback(
        _In_ HQUIC Connection, _In_opt_ void *Context,
        _Inout_ QUIC_CONNECTION_EVENT *Event) {
    return reinterpret_cast<ClientListener *>(Context)->ConnectionCallback(
        Connection, Context, Event);
};


//Client Stream Callback
_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                                     _Inout_ QUIC_STREAM_EVENT *Event) {
    UNREFERENCED_PARAMETER(Context);
    switch (Event->Type) {
    case QUIC_STREAM_EVENT_START_COMPLETE:
        if (QUIC_SUCCEEDED(Event->START_COMPLETE.Status)) {
            printf("Stream started successfully.\n");
            Event->START_COMPLETE.PeerAccepted = TRUE;
        } else {
            printf("Stream start failed, 0x%x!\n",
                   Event->START_COMPLETE.Status);
        }
        break;
    case QUIC_STREAM_EVENT_SEND_COMPLETE:
        free(Event->SEND_COMPLETE.ClientContext);
        printf("[strm][%p] Data sent\n", Stream);
        break;
    case QUIC_STREAM_EVENT_RECEIVE: {
        QUIC_STATUS Status = QUIC_STATUS_SUCCESS;

        auto data = Event->RECEIVE.Buffers->Buffer;
        size_t dataSize = Event->RECEIVE.TotalBufferLength;

        std::printf("\n\nReceived Cord from [%p]", Stream);

        // HandlePeer(Stream, (*data), dataSize);

        MsQuic->StreamReceiveComplete(Stream, Event->RECEIVE.TotalBufferLength);
    } break;
    case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
        printf("[strm][%p] SEND Peer aborted\n", Stream);
        break;
    case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
        printf("[strm][%p] RECEIVE Peer aborted\n", Stream);
        break;
    case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN: {
        printf("[strm][%p] Peer shut down\n", Stream);

        // threadPool.enqueueTask([Stream, this](HQUIC, void *) -> bool {
        //     return onPeerShutdown(static_cast<HQUIC>(Stream),
        //                           static_cast<void *>(this));
        // });
    } break;
    case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
        printf("[strm][%p] All done\n", Stream);
        if (!Event->SHUTDOWN_COMPLETE.AppCloseInProgress) {
            MsQuic->StreamClose(Stream);
        }
        break;
    case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
        printf("[strm][%p] Send shutdown complete\n", Stream);
        break;
    default:
        printf("Stream Event: %d\n", Event->Type);
        break;
    }
    return QUIC_STATUS_SUCCESS;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
    ClientListener::StaticStreamCallback(_In_ HQUIC Stream,
                                         _In_opt_ void *Context,
                                         _Inout_ QUIC_STREAM_EVENT *Event) {
    return reinterpret_cast<ClientListener *>(Context)->StreamCallback(
        Stream, Context, Event);
};

#pragma region Start()

void ClientListener::Start() {
    if (!this->isRunning.load()) {
        this->isRunning.store(true);

        QUIC_ADDR Address = {0};
        QuicAddrSetFamily(&Address, QUIC_ADDRESS_FAMILY_UNSPEC);
        QuicAddrSetPort(&Address, UdpPort);
        printf("Starting listener on port %d\n", UdpPort);
        HQUIC Registration = this->Registration;
        const QUIC_BUFFER Alpn = this->Alpn;

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
