#pragma once
#include <functional>
#include <unordered_map>
#include "../proto/build/wrapper.pb.h"
#include "../proto/build/contact.pb.h"
#include "../proto/build/message.pb.h"
#include "../proto/build/user.pb.h"

class RouteManager {
  public:
  protected:
    bool proccesRoute(const Wrapper&);
    virtual ~RouteManager();
  private:
    using Payload = std::variant<User, Message, Contact>;
    using RouteFunction = std::function<bool(const Payload&)>;
    static std::unordered_map<uint8_t, RouteFunction>* routes;
};