#pragma once

#include <list>
#include <atomic>

#include <boost/asio.hpp>

#include "NetBase.h"

namespace NLNET {

class UnifiedConnection;


class NetClient : public NetBase
{
public:
    NetClient(boost::asio::io_service& io_service, UnifiedConnection& conn);
    virtual ~NetClient();

    virtual void send(CMessage msg, TSockPtr sock) override;

    virtual bool flush(UnifiedConnectionPtr conn) override;
    virtual void update(DiffTime diff_time) override;
    virtual bool connected() const override;
    virtual void disconnect(UnifiedConnectionPtr conn) override;

    bool connect(const CInetAddress& addr);
    bool reconnect();

    TSockPtr getSock();
private:
    bool syncConnect(const std::string& ip, int32_t port);

private:
    boost::asio::io_service& m_io_service;
    TSockPtr                 m_sock;
    std::list<CMessage>      m_write_msgs;
    std::atomic<bool>        m_is_connected;
    CInetAddress             m_address;
};

}
