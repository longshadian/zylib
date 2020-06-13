#pragma once

#include <cstdint>
#include <string>
#include <sstream>
    
namespace zysoft
{
namespace net
{

class InetAddress
{
public:
    InetAddress(std::string ip, std::uint16_t port)
        : ip_(std::move(ip))
        , port_(port)
    {
    }

    ~InetAddress()
    {
    }

    InetAddress(const InetAddress&) = default;
    InetAddress& operator=(const InetAddress&) = default;
    InetAddress(InetAddress&&) = default;
    InetAddress& operator=(InetAddress&&) = default;

    std::string IpPort_ToString() const
    {
        std::ostringstream ostm{};
        ostm << ip_ << ":" << port_;
        return ostm.str();
    }

    std::string ip_;
    std::uint16_t port_;
};

}  // namespace net
}  // namespace muduo

