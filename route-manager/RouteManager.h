#pragma once
#include <functional>
#include <unordered_map>
#include "../proto/build/wrapper.pb.h"

using RouteFunction = std::function<bool(const Wrapper&)>;

class RouteManager {
  public:
  protected:
    bool proccesRoute();
  private:
    static std::unordered_map<uint8_t, RouteFunction>* routes;
};