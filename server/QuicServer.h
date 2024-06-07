#pragma once
#include "../MsQuic/include/msquic.h"
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <sys/stat.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <absl/strings/cord.h>
#include "../proto/build/user.pb.h"
#include "../Helpers/ConnectionManager.h"
#include "../Helpers/PeerHandler.h"

class QuicServer {

public:
	const QUIC_API_TABLE* MsQuic;
	static std::condition_variable cv;
	static std::condition_variable server_status;
	static std::mutex cv_m;
	static std::mutex server_status_m;
	static bool disconnected;

	void Start();

	void Close();

	bool getIsRunning();

	void ServerLoadConfiguration(const char* hash);

	void ServerLoadConfiguration(const char* cert, const char* key);

	static void xsend(HQUIC, void*);

	void send(HQUIC);

	QuicServer(const char* Host, const uint16_t UdpPort, const char* cert, const char* key = nullptr);

	_IRQL_requires_max_(PASSIVE_LEVEL) _Function_class_(QUIC_LISTENER_CALLBACK)
	QUIC_STATUS QUIC_API ServerListenerCallback(
		_In_ HQUIC Listener,
		_In_opt_ void* Context,
		_Inout_ QUIC_LISTENER_EVENT* Event
	);

	static QUIC_STATUS QUIC_API StaticServerListenerCallback(
		_In_ HQUIC Listener,
		_In_opt_ void* Context,
		_Inout_ QUIC_LISTENER_EVENT* Event
	);

	_IRQL_requires_max_(DISPATCH_LEVEL) _Function_class_(QUIC_CONNECTION_CALLBACK)
		QUIC_STATUS QUIC_API ServerConnectionCallback(
			_In_ HQUIC Connection,
			_In_opt_ void* Context,
			_Inout_ QUIC_CONNECTION_EVENT* Event
	);

	static QUIC_STATUS QUIC_API StaticServerConnectionCallback(
		_In_ HQUIC Connection,
		_In_opt_ void* Context,
		_Inout_ QUIC_CONNECTION_EVENT* Event
	);

	_IRQL_requires_max_(DISPATCH_LEVEL) _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API ClientStreamCallback(
		_In_ HQUIC Stream,
		_In_opt_ void* Context,
		_Inout_ QUIC_STREAM_EVENT* Event
	);

	static QUIC_STATUS QUIC_API StaticClientStreamCallback(
		_In_ HQUIC Stream,
		_In_opt_ void* Context,
		_Inout_ QUIC_STREAM_EVENT* Event
	);
	~QuicServer();
private :
	
	QUIC_CREDENTIAL_CONFIG CredConfig;
	uint16_t UdpPort;
	HQUIC Connection;
	HQUIC Registration;
	HQUIC Configuration;
	HQUIC Listener = NULL;
	QUIC_STATUS Status;
	char* Host;
	char* cert;
	char* key;
	const QUIC_REGISTRATION_CONFIG RegConfig = { "Server", QUIC_EXECUTION_PROFILE_TYPE_REAL_TIME };
	const QUIC_BUFFER Alpn = { sizeof("nexus") - 1, (uint8_t*)"nexus" };//wq-vvv-01
	std::atomic<bool> isRunning = false;
	std::thread serverThread;

	uint32_t DecodeHexBuffer(
		_In_z_ const char* HexBuffer,
		_In_ uint32_t OutBufferLen,
		_Out_writes_to_(OutBufferLen, return)
		uint8_t* OutBuffer
	);

	uint8_t DecodeHexChar(_In_ char c);

	QuicServer();

};