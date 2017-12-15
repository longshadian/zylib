#pragma once

#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <memory>
#include <functional>
#include <thread>

#include <boost/asio.hpp>

#include "NetworkType.h"

class ServerCallback;

struct ServerOption
{
    size_t m_max_connection{65535};    //0:unlimited
    size_t m_timeout_seconds{0};       //0:never timeout
};

class StreamServer
{
    friend class RWHandler;
public:
    StreamServer(uint16_t port, ServerOption optin = {});
    ~StreamServer();
    StreamServer(const StreamServer& rhs) = delete;
    StreamServer& operator=(const StreamServer& rhs) = delete;
    StreamServer(StreamServer&& rhs) = delete;
    StreamServer& operator=(StreamServer&& rhs) = delete;

    void Start();
    void Stop();

    boost::asio::io_service& getIOService();
    const ServerOption& getOption() const;

    void SetCallback(std::unique_ptr<ServerCallback> cb);
    ServerCallback& GetCallback();

private:
    void Run();
    void acceptCallback(boost::system::error_code ec);
    void stopHandler(const RWHandlerPtr& handler);
    void stopAccept();
    RWHandlerPtr createHandler();
            
private:
    std::thread                         m_thread;
    boost::asio::io_service             m_io_service;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    boost::asio::ip::tcp::acceptor      m_acceptor;
    boost::asio::ip::tcp::socket        m_socket;
    std::unordered_set<RWHandlerPtr>    m_handlers;
    ServerOption                        m_option;

    std::unique_ptr<ServerCallback>     m_callback;
};
