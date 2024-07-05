#include <iostream>

class RequestManager {
  private:
    RequestManager();
  protected:
#pragma region Auth Requests
    void StartSignUpRequest();
    void StartSignInRequest();
#pragma endregion

#pragma region Send Requests
    void SendMessageToUser();
    void SendMessageToChannel();
#pragma endregion
    virtual ~RequestManager();
};