#pragma once

#include <array>
#include <vector>

#include "network/RWHandler.h"

class ServerHandler : public network::RWHandler
{
public:
    ServerHandler(network::AsyncServer& server, boost::asio::ip::tcp::socket socket);
    virtual ~ServerHandler();

    virtual void start() override;
    virtual void handlerAccept(network::ConnectionHdl hdl) override;
    virtual void handlerClosed(network::ConnectionHdl hdl) override;
    virtual void handlerTimeout(network::ConnectionHdl hdl) override;
private:
    void readHead();
    void readBody();
private:
    std::array<uint8_t, 4 + 4>  m_read_head;
    std::vector<uint8_t>    m_read_body;

    int32_t m_total_len;                  
    int32_t m_msg_id;
};

class ServerHandlerFactory : public network::RWHandlerFactory
{
public:
    ServerHandlerFactory();
    virtual ~ServerHandlerFactory();

    virtual std::shared_ptr<network::RWHandler> create(network::AsyncServer& server,
        boost::asio::ip::tcp::socket socket);
};
