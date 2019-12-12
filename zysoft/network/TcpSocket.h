#pragma once

#include <memory>
#include <boost/asio.hpp>

#include "network/IOContextPool.h"

namespace network
{

class TcpSocket
{
public:
    TcpSocket(IOContextPtr ioc)
        : m_ioc(ioc)
        , m_socket(ioc->m_ioctx)
    {
    }

    ~TcpSocket()
    {
    }

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    IOContextPtr                                    m_ioc;
    boost::asio::ip::tcp::socket                    m_socket;
};

using TcpSocketPtr = std::shared_ptr<TcpSocket>;

} // namespace network
