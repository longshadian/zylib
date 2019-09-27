#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>

#include <boost/asio.hpp>
#include "network/Define.h"
#include "network/Event.h"
#include "network/IOContextPool.h"
#include "network/Message.h"
#include "network/Channel.h"

namespace network
{

class TcpServer;
using TcpServerPtr = std::shared_ptr<TcpServer>;

struct ServerOption
{
    std::size_t     m_max_connection{65535};    //0:unlimited
    std::uint32_t   m_read_timeout_seconds{0};
    std::uint32_t   m_write_timeout_seconds{0};
};

class TcpServer : public std::enable_shared_from_this<TcpServer>
{
public:
    TcpServer(NetworkFactoryPtr fac, std::string host, std::uint16_t port, ServerOption option = {});
    ~TcpServer();
    TcpServer(const TcpServer& rhs) = delete;
    TcpServer& operator=(const TcpServer& rhs) = delete;
    TcpServer(TcpServer&& rhs) = delete;
    TcpServer& operator=(TcpServer&& rhs) = delete;

    static TcpServerPtr Create(NetworkFactoryPtr fac, std::string host, std::uint16_t port, ServerOption opt = {});

    bool Start(std::int32_t n);
    void Stop();
    void StopAccept();

    const ServerOption& GetOption() const;
private:
    bool InitAcceptor(const std::string& host, std::uint16_t port);
    void DoAccept();

private:
    std::string                                 m_host;
    std::uint16_t                               m_port;
    ServerOption                                m_option;
    IOContextPool                               m_accept_pool;
    IOContextPool                               m_io_pool;
    NetworkFactoryPtr                           m_event_factory;
    NetworkEventPtr                             m_event;
    TcpAcceptorPtr                              m_acceptor;
    std::atomic<bool>                           m_listening;
};

} // namespace network

