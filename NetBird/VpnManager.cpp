#include "VpnManager.h"

std::set<std::string> VpnManager::getInterfaces(){
    std::set<std::string> interfaces = {};

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");

    boost::system::error_code ec;
    auto endpoints = resolver.resolve(query, ec);

    if (ec) {
        std::cerr << "Failed to resolve: " << ec.message() << std::endl;
        return interfaces;
    }

    for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
        boost::asio::ip::tcp::endpoint ep = *it;
        std::string ipAddress = ep.address().to_string();
        interfaces.insert(ipAddress);
    }

    return interfaces;
}