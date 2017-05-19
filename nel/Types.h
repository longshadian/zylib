#pragma once

#include <vector>
#include <memory>

#include <boost/asio.hpp>

namespace NLNET {

using TcpSocket = boost::asio::ip::tcp::socket;
using UdpSocket = boost::asio::ip::udp::socket;

using CMessage = std::vector<uint8_t>;
using DiffTime = uint32_t;

using SID = uint32_t;

struct CInetAddress
{
    std::string m_ip;
    std::string m_port;
};

class CUnifiedConnection;
using CUnifiedConnectionPtr = std::shared_ptr<CUnifiedConnection>;

struct ServiceAddr
{
    std::string m_service_name;
    SID         m_sid;
    std::vector<CInetAddress> m_addresses;
};


}
