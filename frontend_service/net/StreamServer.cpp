#include "StreamServer.h"
#include <cassert>

#include "RWHandler.h"
#include "FakeLog.h"

StreamServer::StreamServer(uint16_t port, ServerOption option)
    : m_thread()
    , m_io_service()
    , m_work()
    , m_acceptor(m_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_socket(m_io_service)
    , m_handlers()
    , m_option(option)
    , m_callback()
{
}

StreamServer::~StreamServer()
{
    Stop();
    if (m_thread.joinable())
        m_thread.join();
}

void StreamServer::Start()
{
    if (m_work)
        return;
    m_work = std::make_unique<boost::asio::io_service::work>(m_io_service);
    m_acceptor.async_accept(m_socket, std::bind(&StreamServer::acceptCallback, this, std::placeholders::_1));
    std::thread temp{std::bind(&StreamServer::Run, this)};
    m_thread = std::move(temp);
}

void StreamServer::Stop()
{
    stopAccept();
    m_io_service.stop();
}

void StreamServer::stopAccept()
{
    boost::system::error_code ec;
    m_acceptor.cancel(ec);
    m_acceptor.close(ec);
}

std::shared_ptr<RWHandler> StreamServer::createHandler()
{
    if (m_option.m_max_connection != 0 && m_handlers.size() >= m_option.m_max_connection) {
        GetCallback().HandlerAcceptOverflow();
        return nullptr;
    }
    auto handler = std::make_shared<RWHandler>(std::move(m_socket), *this);
    handler->init();
    m_handlers.insert(handler);
    return handler;
}

void StreamServer::Run()
{
    try {
        m_io_service.run();
    } catch (...) {
    }
}

void StreamServer::acceptCallback(boost::system::error_code ec)
{
    if (!ec) {
        RWHandlerPtr handler = createHandler();
        if (handler) {
            GetCallback().HandlerAccept(handler->getHdl());
        } else {
            // refulse accept
            auto this_socket = std::move(m_socket);
            boost::system::error_code ec_2{};
            this_socket.shutdown(boost::asio::socket_base::shutdown_both, ec_2);
            this_socket.close(ec_2);
        }
        FAKE_LOG(DEBUG) << "current handler:" << m_handlers.size();
    } else {
        FAKE_LOG(DEBUG) << "accept error. server will stop accept. reason:" << ec.value() << " "  << ec.message();
        stopAccept();
        return;
    }
    Start();
}

void StreamServer::stopHandler(const RWHandlerPtr& handler)
{
    m_handlers.erase(handler);
    FAKE_LOG(DEBUG) << "current connect count:" << m_handlers.size();
}

boost::asio::io_service& StreamServer::getIOService()
{
    return m_acceptor.get_io_service();
}

const ServerOption& StreamServer::getOption() const
{
    return m_option;
}

void StreamServer::SetCallback(std::unique_ptr<ServerCallback> cb)
{
    m_callback = std::move(cb);
}

ServerCallback& StreamServer::GetCallback()
{
    return *m_callback;
}
