#include "QuicClient.h"
#include "clientListenerModule/ClientListener.h"
#include <cstdint>
#include "../route-manager/Routes.hpp"

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (void)(P)
#endif

std::condition_variable QuicClient::cv;
std::mutex QuicClient::cv_m;
bool QuicClient::disconnected = false;

bool fileExists(const char *file) {
    struct stat buffer;
    return (stat(file, &buffer) == 0);
}
// Test
typedef struct QUIC_CREDENTIAL_CONFIG_HELPER {
    QUIC_CREDENTIAL_CONFIG CredConfig;
    union {
        QUIC_CERTIFICATE_HASH CertHash;
        QUIC_CERTIFICATE_HASH_STORE CertHashStore;
        QUIC_CERTIFICATE_FILE CertFile;
        QUIC_CERTIFICATE_FILE_PROTECTED CertFileProtected;
    };
} QUIC_CREDENTIAL_CONFIG_HELPER;

uint8_t QuicClient::DecodeHexChar(_In_ char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    return 0;
}

uint32_t QuicClient::DecodeHexBuffer(_In_z_ const char *HexBuffer,
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


#pragma region Connect()
void QuicClient::Connect() {
    if (QUIC_FAILED(Status = MsQuic->ConnectionOpen(
                        Registration,
                        QuicClient::StaticClientConnectionCallback, this,
                        &Connection))) {
        printf("ConnectionOpen failed, 0x%x!\n", Status);
    }
    printf("\n[conn][%p] Connecting...\n", Connection);
    printf("\nPort: %u\n", this->UdpPort);
    printf("\nHost: %s\n", this->Host);

    if (ResumptionTicket) {
        if (QUIC_FAILED(Status = MsQuic->SetParam(
                            Connection, QUIC_PARAM_CONN_RESUMPTION_TICKET,
                            ResumptionTicketLength, ResumptionTicket))) {
            printf(
                "SetParam(QUIC_PARAM_CONN_RESUMPTION_TICKET) failed, 0x%x!\n",
                Status);
            goto Error;
        }
    }

    if (QUIC_FAILED(Status = MsQuic->ConnectionStart(
                        Connection, Configuration, QUIC_ADDRESS_FAMILY_UNSPEC,
                        this->Host, this->UdpPort))) {
        printf("ConnectionStart failed, 0x%x!\n", Status);
        goto Error;
    }

Error:
    if (QUIC_FAILED(Status) && Connection != NULL) {
        MsQuic->ConnectionClose(Connection);
    }
}
#pragma endregion

void QuicClient::Disconnect() {
    if (Connection != NULL) {
        MsQuic->ConnectionShutdown(Connection,
                                   QUIC_CONNECTION_SHUTDOWN_FLAG_NONE,
                                   0); // QUIC_CONNECTION_SHUTDOWN_FLAG_NONE
    }
    if(cListener) {
        cListener->Close();
    }
}

#pragma region ClientStreamCallback
_IRQL_requires_max_(DISPATCH_LEVEL)
    _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
    QuicClient::ClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                                     _Inout_ QUIC_STREAM_EVENT *Event) {
    UNREFERENCED_PARAMETER(Context);
    switch (Event->Type) {
    case QUIC_STREAM_EVENT_SEND_COMPLETE:
        free(Event->SEND_COMPLETE.ClientContext);
        printf("[strm][%p] Data sent\n", Stream);
        break;
    case QUIC_STREAM_EVENT_RECEIVE:
        printf("[strm][%p] Data received\n", Stream);
        break;
    case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
        printf("[strm][%p] Peer aborted\n", Stream);
        break;
    case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
        printf("[strm][%p] Peer shut down\n", Stream);
        break;
    case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
        printf("[strm][%p] All done\n", Stream);
        if (!Event->SHUTDOWN_COMPLETE.AppCloseInProgress) {
            MsQuic->StreamClose(Stream);
        }
        break;
    default:
        break;
    }
    return QUIC_STATUS_SUCCESS;
}
#pragma endregion

QUIC_STATUS QUIC_API QuicClient::StaticClientStreamCallback(
    _In_ HQUIC Stream, _In_opt_ void *Context,
    _Inout_ QUIC_STREAM_EVENT *Event) {
    return reinterpret_cast<QuicClient *>(Context)->ClientStreamCallback(
        Stream, Context, Event);
}

void QuicClient::ClientLoadConfiguration(const char *cert, const char *key) {
    QUIC_SETTINGS Settings = {0};

    Settings.IdleTimeoutMs = 0;
    Settings.IsSet.IdleTimeoutMs = TRUE;

    QUIC_CREDENTIAL_CONFIG CredConfig;
    memset(&CredConfig, 0, sizeof(CredConfig));
#pragma region OpenSslCert
    QUIC_CREDENTIAL_CONFIG_HELPER Config;
    memset(&Config, 0, sizeof(Config));
    Config.CredConfig.Flags = QUIC_CREDENTIAL_FLAG_CLIENT;

    if (cert != nullptr && key != nullptr) {
        printf("cert.pem exists: %d\n", fileExists(cert));
        printf("key.pem exists: %d\n", fileExists(key));

        const char *Cert = cert;
        const char *KeyFile = key;

        Config.CertFile.CertificateFile = (char *)Cert;
        Config.CertFile.PrivateKeyFile = (char *)KeyFile;
        Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
        Config.CredConfig.CertificateFile = &Config.CertFile;
        Config.CredConfig.Flags |=
            QUIC_CREDENTIAL_FLAG_INDICATE_CERTIFICATE_RECEIVED |
            QUIC_CREDENTIAL_FLAG_DEFER_CERTIFICATE_VALIDATION;
    }
#pragma endregion

    QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
    if (QUIC_FAILED(Status = MsQuic->ConfigurationOpen(
                        Registration, &Alpn, 1, &Settings, sizeof(Settings),
                        NULL, &Configuration))) {
        printf("ConfigurationOpen failed, 0x%x!\n", Status);
    }

    if (QUIC_FAILED(Status = MsQuic->ConfigurationLoadCredential(
                        Configuration, &Config.CredConfig))) {
        printf("ConfigurationLoadCredential failed, 0x%x!\n", Status);
    }
}

QuicClient::QuicClient(const char *Host, const uint16_t UdpPort,
                       const char *Alpn, const char *cert, const char *key)
    : Alpn{static_cast<uint32_t>(strlen(Alpn)), (uint8_t *)Alpn} {

    this->Host = Host;
    this->UdpPort = UdpPort;

    if (QUIC_FAILED(Status = MsQuicOpen2(&MsQuic))) {
        printf("MsQuicOpen2 failed, 0x%x!\n", Status);
        exit(-1);
    }

    if (QUIC_FAILED(Status =
                        MsQuic->RegistrationOpen(&RegConfig, &Registration))) {
        printf("RegistrationOpen failed, 0x%x!\n", Status);
        exit(-1);
    }

    ClientLoadConfiguration(cert, key);

    cListener = new ClientListener(MsQuic);
    cListener->Start(this->Registration, 6123, this->Alpn);

    HQUIC Connection = NULL;
}

#pragma region ClientConnectionCallback
_IRQL_requires_max_(PASSIVE_LEVEL)
    _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
    QuicClient::ClientConnectionCallback(_In_ HQUIC Connection,
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
        if (ResumptionTicket) {
            free(ResumptionTicket);
            ResumptionTicket = nullptr;
            ResumptionTicketLength = 0;
        }

        // Allocate memory for the new ticket
        ResumptionTicketLength =
            Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength;
        ResumptionTicket = (uint8_t *)malloc(ResumptionTicketLength);
        if (ResumptionTicket == nullptr) {
            printf("Failed to allocate memory for ResumptionTicket\n");
            break;
        }
        
        memcpy(ResumptionTicket,
               Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicket,
               ResumptionTicketLength);

        printf("Resumption ticket stored.\n");

        break;
    default:
        break;
    }
    return QUIC_STATUS_SUCCESS;
}
#pragma endregion

QUIC_STATUS QUIC_API QuicClient::StaticClientConnectionCallback(
    _In_ HQUIC Connection, _In_opt_ void *Context,
    _Inout_ QUIC_CONNECTION_EVENT *Event) {
    return reinterpret_cast<QuicClient *>(Context)->ClientConnectionCallback(
        Connection, Context, Event);
}


#pragma region send(const absl::Cord &message)
void QuicClient::send(const absl::Cord &message) {
    HQUIC Stream = NULL;
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

    if (QUIC_FAILED(
            Status = MsQuic->StreamOpen(Connection, QUIC_STREAM_OPEN_FLAG_NONE,
                                        QuicClient::StaticClientStreamCallback,
                                        this, &Stream))) {
        printf("StreamOpen failed, 0x%x!\n", Status);
        goto Error;
    }

    printf("[strm][%p] Starting...\n", Stream);

    if (QUIC_FAILED(Status = MsQuic->StreamStart(
                        Stream, QUIC_STREAM_START_FLAG_NONE))) {
        printf("StreamStart failed, 0x%x!\n", Status);
        MsQuic->StreamClose(Stream);
        goto Error;
    }

    printf("[strm][%p] Sending data...\n", Stream);

    if (QUIC_FAILED(Status =
                        MsQuic->StreamSend(Stream, SendBuffer, 1,
                                           QUIC_SEND_FLAG_FIN, SendBuffer))) {
        printf("StreamSend failed, 0x%x!\n", Status);
        goto Error;
    }
    std::cout << "\nSucces"
              << "\n";
Error:
    if (QUIC_FAILED(Status)) {
        MsQuic->ConnectionShutdown(Connection,
                                   QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
        free(SendBuffer);
    }
}
#pragma endregion

HQUIC QuicClient::getConnection() { return Connection; }

#pragma region openTunnel()
bool QuicClient::openTunnel() {

    if (QUIC_FAILED(
            Status = MsQuic->StreamOpen(Connection, QUIC_STREAM_OPEN_FLAG_NONE,
                                        QuicClient::StaticClientStreamCallback,
                                        this, &TunnelStream))) {
        printf("\nFailed to open Tunnel\n");
        return false;
    }

    printf("\nTunnel opened\n");
    return true;
}
#pragma endregion

QuicClient::~QuicClient() {
    if (ResumptionTicket) {
        delete[] ResumptionTicket;
        ResumptionTicket = nullptr;
    }
    if(cListener) {
        delete cListener;
    }
    if (MsQuic) {
        if (Configuration) {
            MsQuic->ConfigurationClose(Configuration);
            Configuration = nullptr;
        }

        if (Registration) {
            MsQuic->RegistrationClose(Registration);
            Registration = nullptr;
        }
        MsQuicClose(MsQuic);
        MsQuic = nullptr;
    }
}


#pragma region AuthRequest()
bool QuicClient::AuthRequest(const absl::Cord &auth_request) {

    QUIC_BUFFER *SendBuffer;
    HQUIC Stream = NULL;

    absl::Cord::ChunkIterator start = auth_request.chunk_begin();
    absl::Cord::ChunkIterator end = auth_request.chunk_end();
    std::vector<uint8_t> buffer;
    for (auto it = start; it != end; ++it) {
        buffer.insert(buffer.end(), it->data(), it->data() + it->size());
    }

    SendBuffer = (QUIC_BUFFER *)malloc(sizeof(QUIC_BUFFER));
    SendBuffer->Length = buffer.size();
    SendBuffer->Buffer = (uint8_t *)malloc(SendBuffer->Length);

    std::copy(buffer.begin(), buffer.end(), SendBuffer->Buffer);

    if (QUIC_FAILED(
            Status = MsQuic->StreamOpen(Connection, QUIC_STREAM_OPEN_FLAG_NONE,
                                        QuicClient::StaticClientStreamCallback,
                                        this, &Stream))) {
        printf("StreamOpen failed, 0x%x!\n", Status);
        goto Error;
    }

    printf("[strm][%p] Starting Auth Request...\n", Stream);

    if (QUIC_FAILED(Status = MsQuic->StreamStart(
                        Stream, QUIC_STREAM_START_FLAG_NONE))) {
        printf("StreamStart failed, 0x%x!\n", Status);
        MsQuic->StreamClose(Stream);
        goto Error;
    }

    printf("[strm][%p] Sending Auth Request...\n", Stream);

    if (QUIC_FAILED(Status =
                        MsQuic->StreamSend(Stream, SendBuffer, 1,
                                           QUIC_SEND_FLAG_FIN, SendBuffer))) {
        printf("StreamSend failed, 0x%x!\n", Status);
        goto Error;
    }

Error:
    if (QUIC_FAILED(Status)) {
        MsQuic->ConnectionShutdown(Connection,
                                   QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
        free(SendBuffer);
    }
    return false;
}
#pragma endregion


#pragma region SignUp()

Lxcode QuicClient::SignUp(const Auth &auth) {
    absl::Cord auth_request;

    Wrapper wrapper;
    *wrapper.mutable_auth() = auth;
    wrapper.set_route(SIGN_UP);
    wrapper.SerializePartialToCord(&auth_request);
    if (AuthRequest(auth_request)) {
        return {true, SUCCESS, "Success"};
    }
    return {false, DB_ERROR_CONNECTION_FAILED, "Failed to connect"};
}

#pragma endregion


#pragma region SignIn()

Lxcode QuicClient::SignIn(const Auth &auth) {
    absl::Cord auth_request;

    Wrapper wrapper;
    *wrapper.mutable_auth() = auth;
    wrapper.set_route(SIGN_IN);
    wrapper.SerializePartialToCord(&auth_request);
    if (AuthRequest(auth_request)) {
        return {true, SUCCESS, "Success"};
    }
    return {false, DB_ERROR_CONNECTION_FAILED, "Failed to connect"};
}

#pragma endregion


#pragma region openPeer()

void QuicClient::openPeer(const char* PeerIp, uint16_t UdpPort) {
    HQUIC p2pConnection = NULL;
    if (QUIC_FAILED(Status = MsQuic->ConnectionOpen(
                        Registration,
                        QuicClient::StaticClientConnectionCallback, this,
                        &p2pConnection))) {
        printf("ConnectionOpen failed, 0x%x!\n", Status);
    }
    printf("\n[conn][%p] Connecting...\n", p2pConnection);
    printf("\nPort: %u\n", UdpPort);

    if (QUIC_FAILED(Status = MsQuic->ConnectionStart(
                        p2pConnection, Configuration, QUIC_ADDRESS_FAMILY_UNSPEC,
                        PeerIp, UdpPort))) {
        printf("ConnectionStart failed, 0x%x!\n", Status);
        goto Error;
    }
Error:
    if (QUIC_FAILED(Status) && p2pConnection != NULL) {
        MsQuic->ConnectionClose(p2pConnection);
    }
}


#pragma endregion


#pragma region sendToPeer()

#pragma endregion

#pragma region closePeer()