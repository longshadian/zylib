#pragma once

#include <cstdint>
#include <string>
#include <boost/asio.hpp>

namespace network
{

class Utilities
{
public:
    static const char* NullString();
    static boost::asio::ip::tcp::endpoint CreateEndpoint(const std::string& host, std::uint16_t port);


    template <typename T>
    static void BZero(T* t)
    {
        //static_assert(std::is_pod<T>::value, "T must be POD!");
        std::memset(t, 0, sizeof(T));
    }

    static std::int64_t NextIndex();
};

} // namespace network
