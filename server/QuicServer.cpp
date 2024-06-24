#include "QuicServer.h"
#include <absl/strings/cord.h>
#include <cstdio>

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (void)(P)
#endif

bool fileExists(const char *file) {
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}

std::condition_variable QuicServer::cv;
std::mutex QuicServer::cv_m;
std::condition_variable QuicServer::server_status;
std::mutex QuicServer::server_status_m;
bool QuicServer::disconnected = false;

typedef struct QUIC_CREDENTIAL_CONFIG_HELPER {
    QUIC_CREDENTIAL_CONFIG CredConfig;
    union {
        QUIC_CERTIFICATE_HASH CertHash;
        QUIC_CERTIFICATE_HASH_STORE CertHashStore;
        QUIC_CERTIFICATE_FILE CertFile;
        QUIC_CERTIFICATE_FILE_PROTECTED CertFileProtected;
    };
} QUIC_CREDENTIAL_CONFIG_HELPER;

uint8_t QuicServer::DecodeHexChar(_In_ char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    return 0;
}

uint32_t QuicServer::DecodeHexBuffer(_In_z_ const char *HexBuffer,
                                     _In_ uint32_t OutBufferLen,
                                     _Out_writes_to_(OutBufferLen, return)
                                         uint8_t *OutBuffer) {
    uint32_t HexBufferLen = (uint32_t)strlen(HexBuffer) / 2;
    if (HexBufferLen > OutBufferLen) {
        return 0;
    }

    for (uint32_t i = 0; i < HexBufferLen; i++) {
        OutBuffer[i] = (DecodeHexChar(HexBuffer[i * 2]) << 4) |
                       DecodeHexChar(HexBuffer[i * 2 + 1]);
    }

    return HexBufferLen;
}

void QuicServer::ServerLoadConfiguration(const char *cert, const char *key) {
    QUIC_SETTINGS Settings = {0};
    Settings.IdleTimeoutMs = 0;
    Settings.IsSet.IdleTimeoutMs = TRUE;

    Settings.ServerResumptionLevel = QUIC_SERVER_RESUME_AND_ZERORTT; // QUIC_SERVER_RESUME_AND_ZERORTT
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

void QuicServer::ServerLoadConfiguration(const char *hash) {

    QUIC_SETTINGS Settings = {0};

    Settings.IdleTimeoutMs = 0;
    Settings.IsSet.IdleTimeoutMs = TRUE;

    Settings.ServerResumptionLevel = QUIC_SERVER_RESUME_AND_ZERORTT;
    Settings.IsSet.ServerResumptionLevel = TRUE;

    Settings.PeerBidiStreamCount = 100;
    Settings.IsSet.PeerBidiStreamCount = TRUE;

#pragma region HashCert
    QUIC_CREDENTIAL_CONFIG_HELPER Config;
    memset(&Config, 0, sizeof(Config));
    Config.CredConfig.Flags =
        QUIC_CREDENTIAL_FLAG_REQUIRE_CLIENT_AUTHENTICATION;

    uint32_t CertHashLen = DecodeHexBuffer(
        hash, sizeof(Config.CertHash.ShaHash), Config.CertHash.ShaHash);
    if (CertHashLen != sizeof(Config.CertHash.ShaHash)) {
        printf("Invalid certificate hash!\n");
        return;
    }
    Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_HASH;
    Config.CredConfig.CertificateHash = &Config.CertHash;
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

QUIC_STATUS QUIC_API QuicServer::StaticServerConnectionCallback(
    _In_ HQUIC Connection, _In_opt_ void *Context,
    _Inout_ QUIC_CONNECTION_EVENT *Event) {
    return reinterpret_cast<QuicServer *>(Context)->ServerConnectionCallback(
        Connection, Context, Event);
}

_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
    QuicServer::ClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
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

        HandlePeer(Stream, (*data), dataSize);

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
        onPeerShutdown(Stream);
        // QuicServer::send(Stream, Context);
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

void QuicServer::send(HQUIC Stream, void *Context) {
    return reinterpret_cast<QuicServer *>(Context)->send(Stream);
}

void QuicServer::send(HQUIC Stream) {
    User u;
    u.set_name("Akzestia");
    u.set_email("akzestia@xxx.com");

    absl::Cord message;
    u.SerializePartialToCord(&message);
    QUIC_BUFFER *SendBuffer;
    absl::Cord::ChunkIterator start = message.chunk_begin();
    absl::Cord::ChunkIterator end = message.chunk_end();
    std::vector<uint8_t> buffer;
    for (auto it = start; it != end; ++it) {
        buffer.insert(buffer.end(), it->data(), it->data() + it->size());
    }

    SendBuffer = (QUIC_BUFFER *)malloc(sizeof(QUIC_BUFFER));
    SendBuffer->Length = buffer.size();
    SendBuffer->Buffer = (uint8_t *)malloc(SendBuffer->Length);

    std::copy(buffer.begin(), buffer.end(), SendBuffer->Buffer);
    if (QUIC_FAILED(Status =
                        MsQuic->StreamSend(Stream, SendBuffer, 1,
                                           QUIC_SEND_FLAG_FIN, SendBuffer))) {
        printf("StreamSend failed, 0x%x!\n", Status);
        MsQuic->StreamShutdown(Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0);
    }
}

bool QuicServer::getUserCreds(HQUIC connection, void *Context) {
    return reinterpret_cast<QuicServer *>(Context)->getUserCreds(connection);
}

bool QuicServer::getUserCreds(HQUIC connection) {
    printf("\nHi form non static\n");
    return true;
}

QUIC_STATUS QUIC_API QuicServer::StaticClientStreamCallback(
    _In_ HQUIC Stream, _In_opt_ void *Context,
    _Inout_ QUIC_STREAM_EVENT *Event) {
    return reinterpret_cast<QuicServer *>(Context)->ClientStreamCallback(
        Stream, Context, Event);
}

#pragma region ServerConnectionCallback
_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    QuicServer::ServerConnectionCallback(_In_ HQUIC Connection,
                                         _In_opt_ void *Context,
                                         _Inout_ QUIC_CONNECTION_EVENT *Event) {
    UNREFERENCED_PARAMETER(Context);
    switch (Event->Type) {
    case QUIC_CONNECTION_EVENT_CONNECTED: {

        printf("[conn][%p] Connected\n", Connection);
        QuicServer::getUserCreds(Connection, Context);

        MsQuic->ConnectionSendResumptionTicket(
            Connection, QUIC_SEND_RESUMPTION_FLAG_NONE, 0, NULL);
        User u;
        addUser(Connection, u);
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
        removeUser(Connection);
    } break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
        printf("[conn][%p] All done\n", Connection);
        MsQuic->ConnectionClose(Connection);
        break;
    case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED: {
        printf("[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream);
        HQUIC Stream = Event->PEER_STREAM_STARTED.Stream;
        MsQuic->SetCallbackHandler(
            Stream, (void *)QuicServer::StaticClientStreamCallback, this);
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

#pragma region ServerListenerCallback
_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
    QuicServer::ServerListenerCallback(_In_ HQUIC Listener,
                                       _In_opt_ void *Context,
                                       _Inout_ QUIC_LISTENER_EVENT *Event) {
    UNREFERENCED_PARAMETER(Listener);
    UNREFERENCED_PARAMETER(Context);
    QUIC_STATUS Status = QUIC_STATUS_NOT_SUPPORTED;
    switch (Event->Type) {
    case QUIC_LISTENER_EVENT_NEW_CONNECTION:
        MsQuic->SetCallbackHandler(
            Event->NEW_CONNECTION.Connection,
            (void *)QuicServer::StaticServerConnectionCallback, this);
        Status = MsQuic->ConnectionSetConfiguration(
            Event->NEW_CONNECTION.Connection, Configuration);
        break;
    case QUIC_LISTENER_EVENT_STOP_COMPLETE: {

    } break;
    default:
        break;
    }
    return Status;
}
#pragma endregion

QUIC_STATUS QUIC_API QuicServer::StaticServerListenerCallback(
    _In_ HQUIC Listener, _In_opt_ void *Context,
    _Inout_ QUIC_LISTENER_EVENT *Event) {
    return reinterpret_cast<QuicServer *>(Context)->ServerListenerCallback(
        Listener, Context, Event);
}

#pragma region Start()
void QuicServer::Start() {
    std::cout << "Start"
              << "\n";
    if (!this->isRunning.load()) {
        this->isRunning.store(true);

        this->serverThread = std::thread([this]() {
            if (QUIC_FAILED(Status = MsQuic->ListenerOpen(
                                Registration,
                                QuicServer::StaticServerListenerCallback, this,
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

            std::cout << "Server is running..." << std::endl;
        });
    }
}
#pragma endregion

#pragma region Close()
void QuicServer::Close() {
    if (this->isRunning.load()) {
        this->isRunning.store(false);
        
        this->server_status.notify_all();

        if (this->serverThread.joinable()) {
            this->serverThread.join();
            std::cout << "Server thread has terminated successfully." << std::endl;
        } else {
            std::cout << "Server thread is not joinable or already joined." << std::endl;
        }
    } else {
        std::cout << "Server is not running." << std::endl;
    }

    std::cout << "isRunning: " << this->isRunning.load() << std::endl;
}
#pragma endregion


bool QuicServer::getIsRunning() { return this->isRunning.load(); }

QuicServer::QuicServer(const char *Host, const uint16_t UdpPort, const char* Alpn,
                       const char *cert, const char *key) : Alpn{static_cast<uint32_t>(strlen(Alpn)), (uint8_t *)Alpn} {
    this->Host = (char *)Host;
    this->UdpPort = UdpPort;
    this->cert = (char *)cert;
    this->key = (char *)key;

    if (QUIC_FAILED(Status = MsQuicOpen2(&MsQuic))) {
        printf("MsQuicOpen2 failed, 0x%x!\n", Status);
        exit(-1);
    }

    if (QUIC_FAILED(Status =
                        MsQuic->RegistrationOpen(&RegConfig, &Registration))) {
        printf("RegistrationOpen failed, 0x%x!\n", Status);
        exit(-1);
    }

    QuicAddrSetFamily(&Address, QUIC_ADDRESS_FAMILY_UNSPEC);
    QuicAddrSetPort(&Address, this->UdpPort);

    if (this->key != nullptr) {
        ServerLoadConfiguration(cert, key);
    } else {
        ServerLoadConfiguration(cert);
    }
}

QuicServer::~QuicServer() {
    std::cout << "Destructor called for QuicServer" << std::endl;

    auto begin = ConnectionManager::getUsers()->begin();
    auto end = ConnectionManager::getUsers()->end();

    std::cout << "Closing existing connections: " << ConnectionManager::getUsers()->size() << std::endl;

    for(;begin != end; begin++){
        printf("\n[conn][%p] Closing\n", begin->first);
        MsQuic->ConnectionShutdown(begin->first, QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
        MsQuic->ConnectionClose(begin->first);
    }

    if (Listener != nullptr) {
        std::cout << "Closing listener..." << std::endl;
        MsQuic->ListenerClose(Listener);
        Listener = nullptr;
    }

    if (MsQuic != nullptr) {
        if (Configuration != nullptr) {
            std::cout << "Closing configuration..." << std::endl;
            MsQuic->ConfigurationClose(Configuration);
            Configuration = nullptr;
        }

        if (Registration != nullptr) {
            std::cout << "Closing registration..." << std::endl;
            MsQuic->RegistrationClose(Registration);
            Registration = nullptr;
        }

        std::cout << "Closing MsQuic..." << std::endl;
        MsQuicClose(MsQuic);
        MsQuic = nullptr;
    }
}
