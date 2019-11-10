#include "network/Utilities.h"

namespace network {

const char* Utilities::NullString()
{
    static const char c = '\0';
    return &c;
}

boost::asio::ip::tcp::endpoint Utilities::CreateEndpoint(const std::string& host, std::uint16_t port)
{
    auto addr = boost::asio::ip::address_v4::from_string(host);
    return boost ::asio::ip::tcp::endpoint { addr, port };
}

std::int64_t Utilities::NextIndex()
{
    static std::atomic<std::int64_t> idx_ = { 0 };
    return ++idx_;
}

} // namesapce network
