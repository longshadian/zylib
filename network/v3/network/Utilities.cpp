#include "Utilities.h"

const char* Utilities::NullStr()
{
    static const char c = '\0';
    return &c;
}

boost::asio::ip::tcp::endpoint Utilities::CreateEndpoint(const std::string& host, std::uint16_t port)
{
    auto addr = boost::asio::ip::address_v4::from_string(host);
    return boost ::asio::ip::tcp::endpoint{addr, port};
}
