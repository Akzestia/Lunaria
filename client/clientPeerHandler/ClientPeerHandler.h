#ifndef CLIENT_PEER_HANDLER_H
#define CLIENT_PEER_HANDLER_H
#include "../../MsQuic/Linux_x64/include/msquic.h"
#include "../QuicResponse.hpp"
#include <condition_variable>
#include <cstdint>
#include <google/protobuf/arena.h>
#include <mutex>
#include <shared_mutex>
#include <sys/stat.h>
#include <unordered_map>

using Arena = google::protobuf::Arena;

enum AuthType : uint8_t {
    T_SIGN_UP = 0,
    T_SIGN_IN = 1,
};
enum ReleaseMutexType : uint8_t {
    T_CONTACT_POST = 2,
    T_CONTACT_PUT = 3,
    T_CONTACT_GET = 4,
    T_CONTACT_DELETE = 5,
};

class ClientPeerHandler {
  public:
    virtual ~ClientPeerHandler();

  protected:

    static Arena* signInArenaRef;
    static Arena* signUpArenaRef;
    static Arena* contactPostArenaRef;
    static Arena* contactGetArenaRef;

    static void SetSignInArena(Arena *);
    static void SetSignUpArena(Arena *);
    static void SetContactPostArena(Arena *);
    static void SetContactGetArena(Arena *);

    static bool waitingForLogin;
    static QuicResponse loginResponse;

    static bool waitingForSignUp;
    static QuicResponse signUpResponse;

    static bool waitingForContact_POST, waitingForContact_PUT,
        waitingForContact_DELETE, waitingForContact_GET;
    static QuicResponse contactResponse_POST, contactResponse_PUT,
        contactResponse_DELETE, contactResponse_GET;

    static bool waitingForServer_POST, waitingForServer_PUT,
        waitingForServer_DELETE, waitingForServer_GET;
    static QuicResponse serverResponse_POST, serverResponse_PUT,
        serverResponse_DELETE, serverResponse_GET;

    static void HandlePeer(HQUIC Stream, const uint8_t &data, size_t dataSize);
    static std::unordered_map<HQUIC, uint8_t *> *GetPeers();
    static void SetPeer(HQUIC Stream, const uint8_t &data, size_t dataSize);
    static bool onPeerShutdown(HQUIC Stream, void *context);
    static void
    ReleaseAuthMutex(std::mutex &lock, std::condition_variable_any &login_Cv,
                     AuthType authType, bool success,
                     const AuthResponse &authResponse = AuthResponse());
    static void
    ReleaseAnyMutex(std::mutex &lock, std::condition_variable_any &variable,
                    ReleaseMutexType release_type, bool succes,
                    const QuicResponse &quic_response = QuicResponse());

    static void ReleaseAll();

    static std::mutex &GetLoginMutex();
    static std::condition_variable_any &GetLoginCv();

    static std::mutex &GetSignUpMutex();
    static std::condition_variable_any &GetSignUpCv();

    static std::mutex &GetContactMutex();
    static std::condition_variable_any &GetContactCv();

  private:
    static std::mutex loginMutex;
    static std::condition_variable_any login_Cv;

    static std::mutex signupMutex;
    static std::condition_variable_any signup_Cv;

    static std::mutex contactMutex;
    static std::condition_variable_any contact_Cv;

    static std::unordered_map<HQUIC, uint8_t *> *peers;
    static std::unordered_map<HQUIC, size_t> *peerDataSizes;
    friend class QuicClient;
};

#endif // !CLIENT_PEER_HANDLER_H
