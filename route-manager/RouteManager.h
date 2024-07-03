#pragma once
#include <functional>
#include <unordered_map>
#include "../proto/build/wrapper.pb.h"


class RouteManager {
  public:
  protected:
    bool proccesRoute();
  private:
    using RouteFunction = std::function<bool(const Wrapper&)>;
    static std::unordered_map<uint8_t, RouteFunction>* routes;
};