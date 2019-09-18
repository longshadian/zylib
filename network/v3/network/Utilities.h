#pragma once

#include <cstdint>
#include <string>
#include <boost/asio.hpp>

class Utilities
{
public:
    static const char* NullStr();
    static boost::asio::ip::tcp::endpoint CreateEndpoint(const std::string& host, std::uint16_t port);
};
