#pragma once

#include <cstdint>

#include <boost/asio.hpp>

#include <net/InetAddress.h>
#include <net/console_log.h>

namespace zysoft
{
namespace net
{
namespace util
{


inline
const char* NullString()
{
    static const char c = '\0';
    return &c;
}

inline
boost::asio::ip::tcp::endpoint CreateEndpoint(const std::string& host, std::uint16_t port)
{
    auto addr = boost::asio::ip::address_v4::from_string(host);
    return boost::asio::ip::tcp::endpoint{ addr, port };
}

template <typename T>
inline
void BZero(T* t)
{
    //static_assert(std::is_pod<T>::value, "T must be POD!");
    std::memset(t, 0, sizeof(T));
}

inline
std::int64_t NextIndex()
{
    static std::atomic<std::int64_t> idx_ = { 0 };
    return ++idx_;
}

inline
const char* Find(const char* b, std::size_t b_len, const char* p, std::size_t p_len)
{
    const char* b_e = b + b_len;
    const char* it = std::search(b, b_e, p, p + p_len);
    if (it == b_e)
        return nullptr;
    return it;
}

inline
const char* Find(const Buffer& buffer, const char* p, std::size_t len)
{
    return Find(buffer.ReadablePtr(), buffer.ReadableBytes(), p, len);
}

inline
const char* FindCRLF(const Buffer& b)
{
    static const char* p = "\r\n";
    return Find(b, p, 2);
}

inline
const char* FindEOL(const Buffer& b)
{
    static const char* p = "\n";
    return Find(b, p, 1);
}

inline
void DefaultConnectionCB(const TcpConnectionPtr& conn)
{
    NET_LOG_TRACE << "Default connection callback. " << conn->GetConnName();
}

inline
void DefaultMessageCB(const TcpConnectionPtr& conn, Buffer* b)
{
    NET_LOG_TRACE << "Default message callback. " << conn->GetConnName() << " buffer bytes: " << b->ReadableBytes();
    b->Clear();
}

inline
void DefaultWriteCompleteCB(const TcpConnectionPtr& conn)
{
    NET_LOG_TRACE << "Default write complete callback. " << conn->GetConnName();
}

inline
void DefaultCloseCB(const TcpConnectionPtr& conn)
{
    NET_LOG_TRACE << "Default close callback. " << conn->GetConnName();
}

inline
void DefaultErrorCB(const TcpConnectionPtr& conn, const ErrorCode& ecode)
{
    NET_LOG_TRACE << "Default error callback. " << conn->GetConnName() << " error code: " << ecode.value() << " reason: " << ecode.message();
}

inline
std::string ToString(const InetAddress& addr, int conn_id)
{
    char buffer[64] = {0};
    snprintf(buffer, sizeof buffer, "%s:%d#%d", addr.ip_.c_str(), static_cast<int>(addr.port_), conn_id);
    return buffer;
}



} // namespace util
} // namespace net
} // namespace zysoft
