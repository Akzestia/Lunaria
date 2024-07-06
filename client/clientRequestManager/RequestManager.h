#include <iostream>
#include "../../proto/build/auth.pb.h"
#include "../../db/DbManager.h"
#include "../../error-manager/ErrorManager.h"

class RequestManager : protected DbManager {
  private:
    RequestManager() = delete;
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