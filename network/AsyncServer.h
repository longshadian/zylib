#pragma once

#include <unordered_map>
#include <unordered_set>
#include <numeric>

#include <boost/asio.hpp>

#include "NetworkType.h"

class AsyncServer
{
public:
    struct Option
    {
        int m_max_connection{65536};    //0:unlimited
        int m_timeout_seconds{0};       //0:never timeout
    };
public:
    AsyncServer(boost::asio::io_service& io_service, short port, const Option& optin);
    ~AsyncServer() = default;

    void accept();
    void stop();
private:
    void handleAcceptError(const boost::system::error_code& ec);
    void stopAccept();
    RWHandlerPtr createHandler();
    void stopHandler(const RWHandlerPtr& conn);

    static void refuseAccept(boost::asio::ip::tcp::socket socket);
private:
    boost::asio::io_service&            m_io_service;
    boost::asio::ip::tcp::acceptor      m_acceptor;
    boost::asio::ip::tcp::socket        m_socket;
    std::unordered_set<RWHandlerPtr>    m_handlers;
    Option                              m_option;
};
