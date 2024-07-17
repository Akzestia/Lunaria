#include <set>
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

class VpnManager{
    public:
        static std::set<std::string> getInterfaces();
    private:
        VpnManager();
 };