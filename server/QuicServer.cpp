#include "QuicServer.h"
#include "../Helpers/fileChecks.hpp"
#include "../route-manager/Routes.hpp"
#include <absl/strings/cord.h>
#include <cstdio>

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (void)(P)
#endif

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

        threadPool.enqueueTask([Stream, this](HQUIC, void *) -> bool {
            return onPeerShutdown(static_cast<HQUIC>(Stream),
                                  static_cast<void *>(this));
        });
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

#pragma region SendResponse(HQUIC Stream,const Wrapper& wrapper)

void QuicServer::SendResponse(HQUIC Stream, const Wrapper &wrapper) {
    absl::Cord response;
    wrapper.SerializePartialToCord(&response);
    QUIC_BUFFER *SendBuffer;
    absl::Cord::ChunkIterator start = response.chunk_begin();
    absl::Cord::ChunkIterator end = response.chunk_end();
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
        printf("Response failed, 0x%x!\n", Status);
        MsQuic->StreamShutdown(Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0);
    }
}

#pragma endregion

void QuicServer::send(HQUIC Stream, void *Context) {
    return reinterpret_cast<QuicServer *>(Context)->send(Stream);
}

std::string QuicAddrToIpString(const QUIC_ADDR &addr) {
    char ipStr[INET6_ADDRSTRLEN];       // Buffer to hold the IP string
    if (addr.Ip.sa_family == AF_INET) { // IPv4
        inet_ntop(AF_INET, &addr.Ipv4.sin_addr, ipStr, sizeof(ipStr));
    } else if (addr.Ip.sa_family == AF_INET6) { // IPv6
        inet_ntop(AF_INET6, &addr.Ipv6.sin6_addr, ipStr, sizeof(ipStr));
    } else {
        return "Unknown AF";
    }
    return std::string(ipStr);
}

// Function to extract port from QUIC_ADDR
uint16_t QuicAddrToPort(const QUIC_ADDR &addr) {
    if (addr.Ip.sa_family == AF_INET) { // IPv4
        return ntohs(addr.Ipv4.sin_port);
    } else if (addr.Ip.sa_family == AF_INET6) { // IPv6
        return ntohs(addr.Ipv6.sin6_port);
    } else {
        return 0; // Unknown AF
    }
}

void QuicServer::openPeer(const char *PeerIp, uint16_t UdpPort) {
    HQUIC p2pConnection = NULL;
    if (QUIC_FAILED(Status = MsQuic->ConnectionOpen(
                        Registration,
                        QuicServer::StaticClientConnectionCallback, this,
                        &p2pConnection))) {
        printf("ConnectionOpen failed, 0x%x!\n", Status);
    }
    printf("\n[conn][%p] Connecting...\n", p2pConnection);
    printf("\nPort: %u\n", UdpPort);

    if (QUIC_FAILED(Status = MsQuic->ConnectionStart(
                        p2pConnection, Configuration,
                        QUIC_ADDRESS_FAMILY_UNSPEC, PeerIp, UdpPort))) {
        printf("ConnectionStart failed, 0x%x!\n", Status);
        goto Error;
    }
Error:
    if (QUIC_FAILED(Status) && p2pConnection != NULL) {
        MsQuic->ConnectionClose(p2pConnection);
    }
}

void QuicServer::SendResponse(const Wrapper &w, const HQUIC &Connection) {
    HQUIC Stream = NULL;
    QUIC_BUFFER *SendBuffer;

    QUIC_ADDR clientAddr = {};
    uint32_t clientAddrSize = sizeof(clientAddr);

    QUIC_STATUS status =
        MsQuic->GetParam(Connection, QUIC_PARAM_CONN_REMOTE_ADDRESS,
                         &clientAddrSize, &clientAddr);
    if (QUIC_SUCCEEDED(status)) {
        std::cout << "Client IP: " << QuicAddrToIpString(clientAddr) << "\n";
        std::cout << "Client Port: " << QuicAddrToPort(clientAddr) << "\n";
    } else {
        std::cerr << "Failed to get client address\n";
    }

    QuicServer::openPeer("localhost", 6122);

    // size_t size = w.ByteSizeLong();
    // std::vector<uint8_t> *buffer = new std::vector<uint8_t>(size);
    // if (!w.SerializeToArray(buffer->data(), buffer->size())) {
    //     std::cerr << "Failed to serialize Wrapper\n";
    //     goto Error;
    // }

    // SendBuffer = (QUIC_BUFFER *)malloc(sizeof(QUIC_BUFFER));
    // SendBuffer->Length = buffer->size();
    // SendBuffer->Buffer = (uint8_t *)malloc(SendBuffer->Length);

    // std::copy(buffer->begin(), buffer->end(), SendBuffer->Buffer);

    // if (QUIC_FAILED(
    //         Status = MsQuic->StreamOpen(Connection,
    //         QUIC_STREAM_OPEN_FLAG_NONE,
    //                                     QuicServer::StaticClientStreamCallback,
    //                                     this, &Stream))) {
    //     printf("StreamOpen failed, 0x%x!\n", Status);
    //     goto Error;
    // }

    // printf("[strm][%p] Starting...\n", Stream);

    // if (QUIC_FAILED(Status = MsQuic->StreamStart(
    //                     Stream, QUIC_STREAM_START_FLAG_NONE))) {
    //     printf("StreamStart failed, 0x%x!\n", Status);
    //     MsQuic->StreamClose(Stream);
    //     goto Error;
    // }

    // printf("[strm][%p] Sending data...\n", Stream);

    // if (QUIC_FAILED(Status =
    //                     MsQuic->StreamSend(Stream, SendBuffer, 1,
    //                                        QUIC_SEND_FLAG_FIN, SendBuffer)))
    //                                        {
    //     printf("StreamSend failed, 0x%x!\n", Status);
    //     goto Error;
    // }

    // std::cout << "\nSuccess" << "\n";

    // delete buffer;

Error:
    if (QUIC_FAILED(Status)) {
        MsQuic->ConnectionShutdown(Connection,
                                   QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
        free(SendBuffer);
        // delete buffer;
    }
};

#pragma region ClientConnectionCallback
_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    QuicServer::ClientConnectionCallback(_In_ HQUIC Connection,
                                         _In_opt_ void *Context,
                                         _Inout_ QUIC_CONNECTION_EVENT *Event) {
    UNREFERENCED_PARAMETER(Context);
    switch (Event->Type) {
    case QUIC_CONNECTION_EVENT_CONNECTED:
        printf("[conn][%p] Connected\n", Connection);
        break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
        if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status ==
            QUIC_STATUS_CONNECTION_IDLE) {
            printf("[conn][%p] Successfully shut down on idle.\n", Connection);
        } else {
            printf("[conn][%p] Shut down by transport, 0x%x\n", Connection,
                   Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
        }
        break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
        printf("[conn][%p] Shut down by peer, 0x%llu\n", Connection,
               (unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
        break;
    case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
        printf("[conn][%p] All done\n", Connection);
        if (!Event->SHUTDOWN_COMPLETE.AppCloseInProgress) {
            MsQuic->ConnectionClose(Connection);
        }
        break;
    case QUIC_CONNECTION_EVENT_RESUMPTION_TICKET_RECEIVED:
        printf("[conn][%p] Resumption ticket received (%u bytes):\n",
               Connection,
               Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength);

        // Free previous ticket if any
        // if (ResumptionTicket) {
        //     free(ResumptionTicket);
        //     ResumptionTicket = nullptr;
        //     ResumptionTicketLength = 0;
        // }

        // // Allocate memory for the new ticket
        // ResumptionTicketLength =
        //     Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength;
        // ResumptionTicket = (uint8_t *)malloc(ResumptionTicketLength);
        // if (ResumptionTicket == nullptr) {
        //     printf("Failed to allocate memory for ResumptionTicket\n");
        //     break;
        // }

        // memcpy(ResumptionTicket,
        //        Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicket,
        //        ResumptionTicketLength);

        printf("Resumption ticket stored.\n");

        break;
    default:
        break;
    }
    return QUIC_STATUS_SUCCESS;
}
#pragma endregion

QUIC_STATUS QUIC_API QuicServer::StaticClientConnectionCallback(
    _In_ HQUIC Connection, _In_opt_ void *Context,
    _Inout_ QUIC_CONNECTION_EVENT *Event) {
    return reinterpret_cast<QuicServer *>(Context)->ClientConnectionCallback(
        Connection, Context, Event);
}

void QuicServer::send(HQUIC Stream) {
    User u;
    u.set_user_name("Akzestia");
    u.set_user_email("akzestia@xxx.com");

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

        // Wrapper w;
        // w.set_route(SERVER_BINDING_REQUEST);
        // QuicServer::SendResponse(w, Connection);

        openPeer("localhost", 6122);

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
            std::cout << "Server thread has terminated successfully."
                      << std::endl;
        } else {
            std::cout << "Server thread is not joinable or already joined."
                      << std::endl;
        }
    } else {
        std::cout << "Server is not running." << std::endl;
    }

    std::cout << "isRunning: " << this->isRunning.load() << std::endl;
}
#pragma endregion

bool QuicServer::getIsRunning() { return this->isRunning.load(); }

QuicServer::QuicServer(const char *Host, const uint16_t UdpPort,
                       const size_t ThreadNumber, const char *Alpn,
                       const char *cert, const char *key)
    : Alpn{static_cast<uint32_t>(strlen(Alpn)), (uint8_t *)Alpn},
      threadPool(ThreadNumber) {

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

    std::cout << "Closing existing connections: "
              << ConnectionManager::getUsers()->size() << std::endl;

    for (; begin != end; begin++) {
        printf("\n[conn][%p] Closing\n", begin->first);
        MsQuic->ConnectionShutdown(begin->first,
                                   QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
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
