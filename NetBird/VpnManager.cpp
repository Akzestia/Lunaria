#include "VpnManager.h"

bool VpnManager::isPrivateIPv4(const boost::asio::ip::address_v4 &address) {
    return (address.to_uint() >= 0x0A000000 &&
            address.to_uint() <= 0x0AFFFFFF) || // 10.0.0.0 to 10.255.255.255
           (address.to_uint() >= 0xAC100000 &&
            address.to_uint() <= 0xAC1FFFFF) || // 172.16.0.0 to 172.31.255.255
           (address.to_uint() >= 0xC0A80000 &&
            address.to_uint() <= 0xC0A8FFFF); // 192.168.0.0 to 192.168.255.255
}

bool VpnManager::isLinkLocalIPv4(const boost::asio::ip::address_v4 &address) {
    return (address.to_uint() >= 0xA9FE0000 &&
            address.to_uint() <= 0xA9FEFFFF); // 169.254.0.0 to 169.254.255.255
}

bool VpnManager::isLinkLocalIPv6(const boost::asio::ip::address_v6 &address) {
    return (address.to_bytes()[0] == 0xfe &&
            (address.to_bytes()[1] & 0xc0) == 0x80);
}

std::set<std::string> VpnManager::getInterfaces() {
    std::set<std::string> interfaces = {};

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(),
                                                "");

    boost::system::error_code ec;
    auto endpoints = resolver.resolve(query, ec);

    if (ec) {
        std::cerr << "Failed to resolve: " << ec.message() << std::endl;
        return interfaces;
    }

    for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
        boost::asio::ip::tcp::endpoint ep = *it;
        boost::asio::ip::address address = ep.address();

        if (!address.is_loopback() && !address.is_unspecified() &&
            !address.is_multicast()) {
            bool exclude = false;
            if (address.is_v4()) {
                boost::asio::ip::address_v4 ipv4Addr = address.to_v4();
                if (isPrivateIPv4(ipv4Addr) || isLinkLocalIPv4(ipv4Addr)) {
                    exclude = true;
                }
            } else if (address.is_v6()) {
                boost::asio::ip::address_v6 ipv6Addr = address.to_v6();
                if (isLinkLocalIPv6(ipv6Addr)) {
                    exclude = true;
                }
            }

            if (!exclude) {
                std::string ipAddress = address.to_string();
                interfaces.insert(ipAddress);
            }
        }
    }

    return interfaces;
}