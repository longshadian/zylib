#include <iostream>
#include <thread>
#include <atomic>
#include <memory>

#include "stream_server/StreamServer.h"
#include "stream_server/NetworkType.h"

class Server
{
public:
    Server()
        : m_io_service()
        , m_work(m_io_service)
        , m_thread()
        , m_server()
    {
        network::ServerOption opt{};
        m_server = std::make_unique<network::AsyncServer>(m_io_service, 23000, opt);
    }

    ~Server() { stop(); waitThreadExit(); }

    void start();
    void stop();
    void waitThreadExit();
private:
    boost::asio::io_service m_io_service;
    boost::asio::io_service::work m_work;
    std::thread m_thread;
    std::unique_ptr<network::AsyncServer> m_server;
};

void Server::start()
{
    m_server->accept();
    m_thread = std::thread([this] { m_io_service.run(); });
}

void Server::stop()
{
    m_server->stop();
}

void Server::waitThreadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

int main()
{
    network::ServerOption opt{};
    (void)opt;

    return 0;
}