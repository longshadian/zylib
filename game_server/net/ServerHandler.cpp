#include "ServerHandler.h"

#include <memory>
#include <functional>
#include <iostream>

#include "network/AsyncServer.h"

#include "Log.h"
#include "GlobalService.h"
#include "NetworkService.h"
#include "World.h"

ServerHandler::ServerHandler(network::AsyncServer& server, boost::asio::ip::tcp::socket socket)
    : RWHandler(server, std::move(socket))
{
}

ServerHandler::~ServerHandler()
{
}

void ServerHandler::start()
{
    RWHandler::start();
    readHead();
}

void ServerHandler::handlerAccept(network::ConnectionHdl hdl)
{
    LOG(DEBUG) << "network accept:" << m_conn_info.m_timeout_seconds;
    Global::getWorld().networkAccept(std::move(hdl));
}

void ServerHandler::handlerClosed(network::ConnectionHdl hdl)
{
    LOG(DEBUG) << "network closed:" << hdl.lock();
    Global::getWorld().networkClosed(std::move(hdl));
}

void ServerHandler::handlerTimeout(network::ConnectionHdl hdl)
{
    LOG(DEBUG) << "network timeout:" << hdl.lock();
    Global::getWorld().networkTimeout(std::move(hdl));
}

void ServerHandler::readHead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_server.getOption().m_timeout_seconds > 0)
        timer = setTimeoutTimer(m_server.getOption().m_timeout_seconds);
    auto self(shared_from_this());
    boost::asio::async_read(getSocket(), boost::asio::buffer(m_read_head),
        [this, self, timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);

        (void)length;
        if (ec) {
            onClosed();
            LOG(WARNING) << "readHead error:" << ec.value() << ":" << ec.message();
            return;
        }

        m_total_len = 0;
        m_msg_id = 0;
        std::memcpy(&m_total_len, m_read_head.data(), 4);
        std::memcpy(&m_msg_id, m_read_head.data() + 4, 4);

        if (m_total_len <= 8 || m_total_len > 1024) {
            onClosed();
            LOG(ERROR) << "readHead msg_len " << m_total_len;
            return;
        }
        m_read_body.resize(m_total_len - 8 + 1);
        readBody();
    });
}

void ServerHandler::readBody()
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_server.getOption().m_timeout_seconds > 0)
        timer = setTimeoutTimer(m_server.getOption().m_timeout_seconds);
    auto self(shared_from_this());
    boost::asio::async_read(getSocket(), boost::asio::buffer(m_read_body.data(), m_read_body.size() - 1),
        [this, self, timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);
        (void)length;
        if (ec) {
            onClosed();
            LOG(ERROR) << "readBody error " << ec.message();
            return;
        }
        std::string s = (const char*)m_read_body.data();
        //LOG(DEBUG) << "receive:" << s;

        auto msg = std::make_shared<NetworkMsg>();
        msg->m_msg_id = m_msg_id;
        msg->m_timestamp = std::chrono::system_clock::now();
        msg->m_data = std::move(s);

        Global::getWorld().networkReceviedMsg(getConnectionHdl(), std::move(msg));

        //Global::getNetworkService().sendMessage(getConnectionHdl(), s);
        m_read_head.fill(0);
        m_read_body.clear();

        readHead();
    });
}

//////////////////////////////////////////////////////////////////////////
ServerHandlerFactory::ServerHandlerFactory()
{
}

ServerHandlerFactory::~ServerHandlerFactory()
{
}

std::shared_ptr<network::RWHandler> ServerHandlerFactory::create(
    network::AsyncServer& server,
    boost::asio::ip::tcp::socket socket)
{
    return std::make_shared<ServerHandler>(server, std::move(socket));
}
