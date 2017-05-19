#pragma once

#include <list>
#include <atomic>

#include <boost/asio.hpp>

#include "NetBase.h"

namespace NLNET {

class NetClient : public NetBase, std::enable_shared_from_this<NetClient>
{
public:
    NetClient(boost::asio::io_service& io_service);
    virtual ~NetClient();

    virtual void send(CMessage buffer, CUnifiedConnectionPtr conn) override;
    virtual uint64_t getReceiveQueueSize() override;
    virtual uint64_t getSendQueueSize() override;

    virtual void displayReceiveQueueStat() override;
    virtual void displaySendQueueStat() override;

    virtual bool flush(CUnifiedConnectionPtr conn) override;
    virtual void update(DiffTime diff_time) override;
    virtual bool connected() const override;
    virtual void disconnect(CUnifiedConnectionPtr conn) override;

    bool connect(const CInetAddress& addr);
private:
    void doWrite();
private:
    boost::asio::io_service& m_io_service;
    TcpSocket                m_socket;
    std::list<CMessage>      m_write_msgs;
    std::atomic<bool>        m_is_connected;
};

}
