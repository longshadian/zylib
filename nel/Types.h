#pragma once

#include <vector>
#include <memory>

#include <boost/asio.hpp>

namespace NLNET {

using ServiceID = int32_t;
//using SockID = int32_t;

using DiffTime = uint32_t;

class TSock;
using TSockPtr = std::shared_ptr<TSock>;
using TSockHdl = std::weak_ptr<TSock>;

struct CMessage
{
    //using DATA = std::vector<uint8_t>;
    using DATA = std::string;

    CMessage(std::string name, std::string content)
        : m_msg_name(std::move(name))
        , m_data(std::move(content))
    {
    }

    CMessage() = default;

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

    const DATA& getData() const &
    {
        return m_data;
    }

    DATA& getData() &
    {
        return m_data;
    }

    DATA getData() &&
    {
        DATA temp = std::move(m_data);
        return temp;
    }

    // TODO …æ≥˝¡Ÿ ±≤‚ ‘¥˙¬Î
    std::vector<uint8_t> serializeToArray()const
    {
        std::vector<uint8_t> buffer{};
        buffer.resize(m_msg_name.size() + 1 + m_data.size());
        size_t pos = 0;
        for (auto c : m_msg_name) {
            buffer[pos] = static_cast<uint8_t>(c);
            ++pos;
        }
        // ≤π0
        buffer[pos] = 0;
        ++pos;

        for (auto c : m_data) {
            buffer[pos] = static_cast<uint8_t>(c);
            ++pos;
        }
        return buffer;
    }

    bool parseFromArray(const std::vector<uint8_t>& buffer)
    {
        size_t pos = 0;
        for (size_t i = 0; i != buffer.size(); ++i) {
            if (buffer[i] == 0) {
                pos = i;
                break;
            }
        }
        const char* p = (const char*)buffer.data();
        m_msg_name.assign(p, p + pos);

        for (size_t i = pos + 1; i != buffer.size(); ++i) {
            m_data.push_back((char)buffer[i]);
        }
        return true;
    }
    
    std::string          m_msg_name;
    DATA m_data;
};

struct NetWorkMessage
{
    NetWorkMessage() = default;
    TSockHdl m_sock_hdl;
    CMessage m_msg;
};

using NetWorkMessagePtr = std::shared_ptr<NetWorkMessage>;

using AddrID = size_t;

enum {AddrID_Default = 0xFF};
enum {InvalidEndpointIndex = -1};

struct CInetAddress
{
    std::string m_ip;
    int16_t     m_port;

    bool operator==(const CInetAddress& rhs) const
    {
        return m_ip == rhs.m_ip && m_port == rhs.m_port;
    }

    bool operator!=(const CInetAddress& rhs) const
    {
        return !(*this == rhs);
    }
};

class UnifiedConnection;
using UnifiedConnectionPtr = std::shared_ptr<UnifiedConnection>;

struct ServiceAddr
{
    std::string m_service_name;
    ServiceID   m_service_id;
    std::vector<CInetAddress> m_addresses;
};

class NetBase;
using NetBasePtr = std::shared_ptr<NetBase>;

class NetServer;
using NetServerPtr = std::shared_ptr<NetServer>;

class NetClient;
using NetClientPtr = std::shared_ptr<NetClient>;

}
