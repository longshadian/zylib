#pragma once

#include <list>
#include <atomic>

#include <boost/asio.hpp>

#include "Types.h"
#include "Address.h"

#include "zylib/Timer.h"

namespace nlnet {

using Connect_Callback = std::function<void(boost::system::error_code, NetClientPtr)>;

class NetClient : public std::enable_shared_from_this<NetClient>
{
public:
    enum STATE
    {
        CONNECTED = 0,      // 已经连接
        IS_CONNECTING = 1,  // 正在连接
        DISCONNECT = 2,     // 未连接
    };
public:
    NetClient(boost::asio::io_service& io_service);
    ~NetClient();

    int32_t getState() const;
    void disconnect();

    bool connect(const CInetAddress& addr, Connect_Callback cb);
    bool reconnect();
    const TSockPtr& getSock() const;
    const CInetAddress& getAddress() const;
private:
    bool syncConnect(const CInetAddress& addr);
    bool asyncConnect(const CInetAddress& addr);
private:
    boost::asio::io_service& m_io_service;
    TSockPtr                 m_sock;
    std::atomic<int32_t>     m_state;
    CInetAddress             m_inet_addr;
    Connect_Callback         m_connect_cb;
};

}
