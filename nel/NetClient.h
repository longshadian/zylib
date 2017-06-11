#pragma once

#include <list>
#include <atomic>

#include <boost/asio.hpp>

#include "Types.h"

namespace NLNET {

class NetClient
{
    using ConnectCallback = std::function<void(TSockPtr)>;
public:
    NetClient(boost::asio::io_service& io_service);
    ~NetClient();

    bool isConnected() const;
    void disconnect();

    bool connect(const CInetAddress& addr);
    bool reconnect();
    void setConnectCallback(ConnectCallback cb);

    TSockPtr& getSock();
private:
    bool syncConnect(const std::string& ip, int32_t port);

private:
    boost::asio::io_service& m_io_service;
    TSockPtr                 m_sock;
    std::atomic<bool>        m_is_connected;
    CInetAddress             m_address;
    ConnectCallback          m_connect_cb;
};

}
