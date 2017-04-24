#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <boost/asio.hpp>

#include "TcpClient.h"

using SMessage = std::string;

class SEndpoint
{
public:
    enum class STATE
    {
        DISCONNECT,
        CONNECTING,
        CONNECTED,
    };
    SEndpoint(boost::asio::io_service& io_service, std::string ip, int port);
    ~SEndpoint();

    void setState(STATE s);
    STATE getState() const;

    bool connect();
    void close();

    void sendMsg(SMessage msg);
private:
    time_t  m_time;

    std::string     m_ip;
    int			    m_port;
    STATE           m_state;
    std::shared_ptr<TcpClient> m_client;

    mutable std::mutex          m_mtx;
    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::resolver m_resolver;
};

using SEndpointPtr = std::shared_ptr<SEndpoint>;


class RouterService
{
public:
    RouterService();
    ~RouterService();

    bool init();
    void heartbeat(uint32_t diff);

    void routerMsg(const std::string& server_name, SMessage msg);
private:
    SEndpointPtr findEndpoint(const std::string& sname);
private:
    std::unordered_map<std::string, SEndpointPtr> m_server_endpoints;
};
