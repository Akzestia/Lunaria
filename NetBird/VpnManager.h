#include <set>
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

class VpnManager{
    public:
        static std::set<std::string> getInterfaces();
    private:
        static bool isPrivateIPv4(const boost::asio::ip::address_v4 &address);
        static bool isLinkLocalIPv4(const boost::asio::ip::address_v4 &address);
        static bool isLinkLocalIPv6(const boost::asio::ip::address_v6 &address);
        VpnManager();
 };