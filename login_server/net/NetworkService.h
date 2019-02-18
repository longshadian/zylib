#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <memory>

#include <boost/asio.hpp>

#include "network/RWHandler.h"

namespace network { class AsyncServer; }

class NetworkService
{
public:
    NetworkService();
    ~NetworkService();

    bool init();
    void waitStop();
    void stop();
    void shutdownHdl(network::ConnectionHdl hdl);
    void sendMessage(network::ConnectionHdl hdl, int32_t msg_id, const std::string& msg);
private:
    boost::asio::io_service      m_io_service;
    std::thread                  m_thread;
    std::shared_ptr<network::AsyncServer> m_server;
    std::shared_ptr<boost::asio::io_service::work>  m_scope_work;
};
