#include "NetworkService.h"

#include <thread>
#include <chrono>

#include "network/AsyncServer.h"
#include "zylib/Tools.h"

#include "Log.h"
#include "Config.h"
#include "ServerHandler.h"

NetworkService::NetworkService()
    : m_io_service()
    , m_thread()
    , m_server()
    , m_scope_work()
{
}

NetworkService::~NetworkService()
{
    stop();
    waitStop();
}

bool NetworkService::init()
{
    LOG(INFO) << "start network service";
    const auto* server_conf = config::instance().getServerConf();
    network::ServerOption option{};
    option.m_max_connection = server_conf->m_max_count;
    option.m_timeout_seconds = server_conf->m_client_timeout;
    m_server.reset(new network::AsyncServer(m_io_service,
        zylib::make_unique<ServerHandlerFactory>(),
        (short)server_conf->m_port,
        option));
    m_server->accept();

    m_scope_work.reset(new boost::asio::io_service::work(m_io_service));

    auto* p_io_service = &m_io_service;
    m_thread = std::thread([this, p_io_service] { p_io_service->run(); });
    return true;
}

void NetworkService::stop()
{
    m_server->stop();
}

void NetworkService::waitStop()
{
    if (m_thread.joinable())
        m_thread.join();
}

void NetworkService::shutdownHdl(network::ConnectionHdl hdl)
{
    auto conn = hdl.lock();
    if (!conn)
        return;
    conn->shutdown();
}

void NetworkService::sendMessage(network::ConnectionHdl hdl, const std::string& msg)
{
    if (msg.empty())
        return;

    auto conn = hdl.lock();
    if (!conn) {
        LOG(DEBUG) << "conn null";
        return;
    }

    int32_t total_len = 4 + msg.size();
    std::vector<uint8_t> buffer{};
    buffer.resize(total_len);
    std::memcpy(buffer.data(), &total_len, 4);
    std::memcpy(buffer.data() + 4, msg.data(), msg.size());
    conn->sendMsg(std::move(buffer));
}

