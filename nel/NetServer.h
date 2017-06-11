#pragma once

#include <functional>

#include <boost/asio.hpp>

#include "Types.h"

namespace NLNET {

using AcceptFailCallback = std::function<void(boost::system::error_code)>;
using AcceptSuccessCallback = std::function<void(TSockPtr)>;

class NetServer : public std::enable_shared_from_this<NetServer>
{
public:
    NetServer(boost::asio::io_service& io_service, const std::string& ip, int port);
    ~NetServer();

    void stop();
    void accept();
    void setAcceptFailCallback(AcceptFailCallback cb);
    void setAcceptSuccessCallback(AcceptSuccessCallback cb);
private:
    boost::asio::io_service&        m_io_service;
    boost::asio::ip::tcp::acceptor  m_acceptor;
    std::atomic<bool>               m_is_connected;
    boost::asio::ip::tcp::socket    m_socket;
    AcceptFailCallback              m_accept_fail_cb;
    AcceptSuccessCallback           m_accept_success_cb;
};

}
