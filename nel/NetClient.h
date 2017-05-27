#pragma once

#include <list>
#include <atomic>

#include <boost/asio.hpp>

#include "NetBase.h"

namespace NLNET {

class NetClient : public NetBase
{
public:
    NetClient(boost::asio::io_service& io_service);
    virtual ~NetClient();

    virtual void send(CMessage msg, TSockPtr sock) override;

    virtual bool flush(CUnifiedConnectionPtr conn) override;
    virtual void update(DiffTime diff_time) override;
    virtual bool connected() const override;
    virtual void disconnect(CUnifiedConnectionPtr conn) override;

    bool connect(const CInetAddress& addr);
private:
    boost::asio::io_service& m_io_service;
    TSockPtr                 m_sock;
    std::list<CMessage>      m_write_msgs;
    std::atomic<bool>        m_is_connected;
};

}
