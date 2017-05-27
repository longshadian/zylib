#pragma once

#include <vector>
#include <memory>

#include <boost/asio.hpp>

namespace NLNET {

using DiffTime = uint32_t;

struct CMessage
{
    std::string getMsgName() const
    {
        return m_msg_name;
    }

    const void* data() const
    {
        return m_data.data();
    }

    size_t size() const
    {
        return m_data.size();
    }
    
    std::string          m_msg_name;
    std::vector<uint8_t> m_data;
};

struct NetWorkMessage
{
    CMessage m_msg;
};

using NetWorkMessagePtr = std::shared_ptr<NetWorkMessage>;

using DiffTime = uint32_t;

using SID = size_t;
using AddrID = size_t;

enum {AddrID_Default = 0xFF};
enum {InvalidEndpointIndex = -1};

struct CInetAddress
{
    std::string m_ip;
    int16_t     m_port;
};

class UnifiedConnection;
using CUnifiedConnectionPtr = std::shared_ptr<UnifiedConnection>;

struct ServiceAddr
{
    std::string m_service_name;
    SID         m_sid;
    std::vector<CInetAddress> m_addresses;
};

class TSock;
using TSockPtr = std::shared_ptr<TSock>;

}
