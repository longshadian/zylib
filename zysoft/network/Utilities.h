#pragma once

#include <cstdint>
#include <string>
#include <boost/asio.hpp>

namespace network
{

class Utilities
{
public:
    static const char* NullString()
    {
        static const char c = '\0';
        return &c;
    }

    static boost::asio::ip::tcp::endpoint CreateEndpoint(const std::string& host, std::uint16_t port)
    {
        auto addr = boost::asio::ip::address_v4::from_string(host);
        return boost ::asio::ip::tcp::endpoint { addr, port };
    }

    template <typename T>
    static void BZero(T* t)
    {
        //static_assert(std::is_pod<T>::value, "T must be POD!");
        std::memset(t, 0, sizeof(T));
    }

    static std::int64_t NextIndex()
    {
        static std::atomic<std::int64_t> idx_ = { 0 };
        return ++idx_;
    }

};

} // namespace network
