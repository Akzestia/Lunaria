#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H
#include <iostream>
#include "../../proto/build/auth.pb.h"
#include "../../db/DbManager.h"

class RequestManager : protected DbManager {
  private:
    RequestManager() : DbManager() {};
    friend class QuicClient;
  protected:
#pragma region Auth Requests
    Lxcode StartSignUpRequest(const Auth &);
    Lxcode StartSignInRequest(const Auth &);
#pragma endregion

#pragma region Send Requests
    void SendMessageToUser();
    void SendMessageToChannel();
#pragma endregion
    virtual ~RequestManager();
};

#endif