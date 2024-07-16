#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

int main() {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");

    boost::system::error_code ec;
    auto endpoints = resolver.resolve(query, ec);

    if (ec) {
        std::cerr << "Failed to resolve: " << ec.message() << std::endl;
        return 1;
    }

    for (auto it = endpoints.begin(); it != endpoints.end(); ++it) {
        boost::asio::ip::tcp::endpoint ep = *it;
        std::string ipAddress = ep.address().to_string();

        // Check if the IP address starts with "100.73"
        if (ipAddress.compare(0, 6, "100.73") == 0) {
            std::cout << "Interface: " << ipAddress << std::endl;
        }
    }

    return 0;
}
