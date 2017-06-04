#pragma once

#include <functional>

#include <boost/asio.hpp>

#include "NetBase.h"

namespace NLNET {

using AcceptFailCB = std::function<void(boost::system::error_code)>;
using AcceptSuccessCB = std::function<void(TSockPtr)>;

class UnifiedConnection;

class NetServer : public NetBase
{
public:
    NetServer(boost::asio::io_service& io_service, const std::string& ip, int port, UnifiedConnection& conn);
    virtual ~NetServer();

    virtual void send(CMessage msg, TSockPtr sock) override;
    virtual bool flush(UnifiedConnectionPtr conn) override;
    virtual void update(DiffTime diff_time) override;
    virtual bool connected() const override;
    virtual void disconnect(UnifiedConnectionPtr conn) override;

    void accept();
    void setAcceptFailCB(AcceptFailCB cb);
    void setAcceptSuccessCB(AcceptSuccessCB cb);
private:
    boost::asio::io_service&        m_io_service;
    UnifiedConnection&              m_conn;
    boost::asio::ip::tcp::acceptor  m_acceptor;
    std::atomic<bool>               m_is_connected;
    boost::asio::ip::tcp::socket    m_socket;
    AcceptFailCB                    m_accept_fail_cb;
    AcceptSuccessCB                 m_accept_success_cb;
};

}
