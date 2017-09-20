#include "StreamClient.h"

#include <functional>

#include "FakeLog.h"
#include "RWHandler.h"

namespace network {

StreamClient::StreamClient(boost::asio::io_service& io_service, const ClientOption& opt)
    : m_io_service(io_service)
    , m_connect_socket()
    , m_ip()
    , m_port()
    , m_option(opt)
    , m_handler()
    , m_async_connect()
    , m_async_closed()
    , m_async_timeout()
    , m_async_receive_mgs()
    , m_async_msg_decode()
{
    setCB_ReceivedMessage(std::bind(&StreamClient::asyncReceivedMessage_Default, std::placeholders::_1, std::placeholders::_2));
    setCB_MessageDecoder(std::bind(&StreamClient::asyncMessageDecode_Default, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    setCB_HandlerTimeout(std::bind(&StreamClient::asyncHandlerTimeout_Default, std::placeholders::_1));
    setCB_HandlerClosed(std::bind(&StreamClient::asyncHandlerClosed_default, std::placeholders::_1));
}

StreamClient::~StreamClient()
{
}

bool StreamClient::connect(std::string ip, uint16_t port, CB_Connect cb)
{
    m_ip = std::move(ip);
    m_port = port;
    m_async_connect = std::move(cb);
    return doConnect();
}

bool StreamClient::reconnect()
{
    return doConnect();
}

void StreamClient::sendMessage(MessagePtr msg)
{
    if (m_handler)
        m_handler->sendMessage(std::move(msg));
}

void StreamClient::shutdown()
{
    if (m_handler)
        m_handler->shutdown();
}

void StreamClient::setCB_ReceivedMessage(CB_ReceivedMessage cb)
{
    m_async_receive_mgs = std::move(cb);
}

void StreamClient::setCB_MessageDecoder(CB_MessageDecoder cb)
{
    m_async_msg_decode = std::move(cb);
}

void StreamClient::setCB_HandlerTimeout(CB_HandlerTimeout cb)
{
    m_async_timeout = std::move(cb);
}

void StreamClient::setCB_HandlerClosed(CB_HandlerClosed cb)
{
    m_async_closed = std::move(cb);
}

Hdl StreamClient::getHdl()
{
    return m_handler->getHdl();
}

bool StreamClient::doConnect()
{
    try {
        m_handler = nullptr;
        boost::asio::ip::address ip_addr{};
        ip_addr.from_string(m_ip);
        boost::asio::ip::tcp::endpoint ep_pair{ ip_addr, m_port };
        m_connect_socket = std::make_unique<boost::asio::ip::tcp::socket>(m_io_service);
        m_connect_socket->async_connect(ep_pair, std::bind(&StreamClient::doConnectCallback, this, std::placeholders::_1));
        return true;
    } catch (std::exception e) {
        LOG(WARNING) << "conn exception " << e.what();
    }
    return false;
}

void StreamClient::doConnectCallback(boost::system::error_code ec)
{
    if (!ec) {
        LOG(DEBUG) << "connect " << m_ip << " " << m_port << " success";

        HandlerOption opt{};
        opt.m_read_timeout_seconds = m_option.m_read_timeout_seconds;
        m_handler = std::make_shared<RWHandler>(std::move(*m_connect_socket), opt);
        m_handler->setCB_AsyncClosed(m_async_closed);
        m_handler->setCB_AsyncTimeout(m_async_timeout);
        m_handler->setCB_AsyncDecode(m_async_msg_decode);
        m_handler->setCB_AsyncReceiveMsg(m_async_receive_mgs);
        m_handler->init();
    } else {
        LOG(WARNING) << "connect " << m_ip << " " << m_port << " fail " << ec.value() << ":" << ec.message();
    }
    // ÊÍ·Åconnect socket
    resetConnectSocket();
    if (m_async_connect)
        m_async_connect(ec, *this);
}

void StreamClient::resetConnectSocket()
{
    if (m_connect_socket) {
        boost::system::error_code ec;
        m_connect_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        m_connect_socket->close(ec);
        m_connect_socket = nullptr;
    }
}

void StreamClient::asyncHandlerClosed_default(Hdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection closed ";
}

void StreamClient::asyncHandlerTimeout_Default(Hdl hdl)
{
    (void)hdl;
    LOG(DEBUG) << "connection timeout ";
}

void StreamClient::asyncMessageDecode_Default(Hdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    (void)hdl;
    (void)buffer;
    (void)out;
    LOG(DEBUG) << "message decoder";
}

void StreamClient::asyncReceivedMessage_Default(Hdl hdl, std::vector<MessagePtr> messages)
{
    (void)hdl;
    (void)messages;
    LOG(DEBUG) << "received message";
}

} // network
