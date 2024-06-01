#pragma once
#include "../Entities/Contact.h"
#include "../Entities/Message.h"
#include "../Entities/User.h"
#include "../MsQuic/include/msquic.h"
#include <absl/strings/cord.h>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <sys/stat.h>

class QuicClient {
public:
  const QUIC_API_TABLE *MsQuic;
  static std::condition_variable cv;
  static std::mutex cv_m;
  static bool disconnected;

  void Connect();

  void Disconnect();

  template <typename T> void send(T message, uint8_t endpoint) {
    HQUIC Stream = NULL;
    QUIC_BUFFER *SendBuffer;

    std::vector<uint8_t> serializedMessage = message.Serialize();

    uint8_t identifier;
    if (std::is_same<T, Message>::value) {
      identifier = 0x01;
    } else if (std::is_same<T, Contact>::value) {
      identifier = 0x02;
    } else if (std::is_same<T, User>::value) {
      identifier = 0x03;
    } else {
      identifier = 0x99;
      return;
    }

    serializedMessage.insert(serializedMessage.begin(), identifier);
    serializedMessage.insert(serializedMessage.begin(), endpoint);

    SendBuffer = (QUIC_BUFFER *)malloc(sizeof(QUIC_BUFFER));
    if (SendBuffer == NULL) {
      printf("SendBuffer allocation failed!\n");
      Status = QUIC_STATUS_OUT_OF_MEMORY;
      goto Error;
    }

    SendBuffer->Buffer = (uint8_t *)malloc(serializedMessage.size());
    if (SendBuffer->Buffer == NULL) {
      printf("SendBuffer->Buffer allocation failed!\n");
      free(SendBuffer);
      Status = QUIC_STATUS_OUT_OF_MEMORY;
      goto Error;
    }

    std::copy(serializedMessage.begin(), serializedMessage.end(),
              SendBuffer->Buffer);
    SendBuffer->Length = serializedMessage.size();

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
      free(SendBuffer->Buffer);
      free(SendBuffer);
      goto Error;
    }
  Error:

    if (QUIC_FAILED(Status)) {
      MsQuic->ConnectionShutdown(Connection, QUIC_CONNECTION_SHUTDOWN_FLAG_NONE,
                                 0);
    }
  }

  void send(const absl::Cord &message);
  void ClientLoadConfiguration(const char *cert, const char *key);

  QuicClient(const char *Host, const uint16_t UdpPort, const char *cert,
             const char *key);

  _IRQL_requires_max_(PASSIVE_LEVEL)
      _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
      ClientConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                               _Inout_ QUIC_CONNECTION_EVENT *Event);

  static QUIC_STATUS QUIC_API
  StaticClientConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                                 _Inout_ QUIC_CONNECTION_EVENT *Event);

  _IRQL_requires_max_(DISPATCH_LEVEL) ~QuicClient();
  _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
      ClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                           _Inout_ QUIC_STREAM_EVENT *Event);

  static QUIC_STATUS QUIC_API
  StaticClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                             _Inout_ QUIC_STREAM_EVENT *Event);

private:
  QUIC_CREDENTIAL_CONFIG CredConfig;
  uint16_t UdpPort;
  HQUIC Connection;
  HQUIC Registration;
  HQUIC Configuration;
  QUIC_STATUS Status;
  char *Host;
  const QUIC_REGISTRATION_CONFIG RegConfig = {
      "Client", QUIC_EXECUTION_PROFILE_TYPE_REAL_TIME};
  const QUIC_BUFFER Alpn = {sizeof("nexus") - 1, (uint8_t *)"nexus"};

  uint32_t DecodeHexBuffer(_In_z_ const char *HexBuffer,
                           _In_ uint32_t OutBufferLen,
                           _Out_writes_to_(OutBufferLen, return)
                               uint8_t *OutBuffer);

  uint8_t DecodeHexChar(_In_ char c);
};