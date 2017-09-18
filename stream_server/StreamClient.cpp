#include "StreamClient.h"

#include <functional>

#include "FakeLog.h"
#include "RWHandler.h"

namespace network {

StreamClient::StreamClient(boost::asio::io_service& io_service, const ClientOption& opt)
    : m_io_service(io_service)
    , m_socket(m_io_service)
    , m_ip()
    , m_port()
    , m_option(opt)
    , m_cb_connect()
    , m_handler()
    , m_cb_closed()
    , m_cb_timeout()
    , m_cb_receive_mgs()
    , m_cb_msg_decode()
{
}

StreamClient::~StreamClient()
{
}

bool StreamClient::connect(std::string ip, uint16_t port, CBConnect cb)
{
    m_ip = std::move(ip);
    m_port = port;
    m_cb_connect = std::move(cb);
    return asyncConnect();
}

void StreamClient::sendMessage(MessagePtr msg)
{
    if (m_handler)
        m_handler->sendMessage(std::move(msg));
}

void StreamClient::setCBReceiveMsg(CBReceivedMessage cb)
{
    m_cb_receive_mgs = std::move(cb);
}

void StreamClient::setCBMessageDecode(CBMessageDecode cb)
{
    m_cb_msg_decode = std::move(cb);
}

void StreamClient::setCBTimeout(CBHandlerTimeout cb)
{
    m_cb_timeout = std::move(cb);
}

void StreamClient::setCBClosed(CBHandlerClosed cb)
{
    m_cb_closed = std::move(cb);
}

ConnectionHdl StreamClient::getHdl()
{
    return m_handler->getHdl();
}

bool StreamClient::asyncConnect()
{
    try {
        boost::asio::ip::address ip_addr{};
        ip_addr.from_string(m_ip);
        boost::asio::ip::tcp::endpoint ep_pair{ ip_addr, m_port };
        m_socket.async_connect(ep_pair,
            [this, self = shared_from_this()](boost::system::error_code ec)
            {
                if (!ec) {
                    LOG(DEBUG) << "connect " << m_ip << " " << m_port << " success";

                    HandlerOption opt{};
                    opt.m_read_timeout_seconds = m_option.m_read_timeout_seconds;
                    m_handler = std::make_shared<RWHandler>(std::move(m_socket), opt);
                    m_handler->setCBClosed(m_cb_closed);
                    m_handler->setCBTimeout(m_cb_timeout);
                    m_handler->setCBDecode(m_cb_msg_decode);
                    m_handler->setCBReceiveMsg(m_cb_receive_mgs);
                } else {
                    LOG(WARNING) << "connect " << m_ip << " " << m_port << " fail " << ec.value() << ":" << ec.message();
                }
                if (m_cb_connect) {
                    m_cb_connect(ec, *this);
                }
            });
        return true;
    } catch (std::exception e) {
        LOG(WARNING) << "conn exception " << e.what();
    }
    return false;
}

} // network
