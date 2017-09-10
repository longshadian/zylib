#include "RWHandler.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "FakeLog.h"
#include "StreamServer.h"
#include "Message.h"

namespace network {

RWHandler::RWHandler(AsyncServer& server, boost::asio::ip::tcp::socket socket)
    : m_server(server)
    , m_io_service(m_server.getIOService())
    , m_socket(std::move(socket))
    , m_write_buffer()
    , m_is_closed(false)
    , m_conn_info()
    , m_read_fix_buffer()
    , m_read_buffer()
{
}

RWHandler::~RWHandler()
{
}

void RWHandler::start()
{
    m_server.cbAccecpt(getHdl());
}

void RWHandler::cbClosed() 
{ 
    m_server.cbClosed(getHdl());
}

void RWHandler::cbTimeout() 
{
    m_server.cbTimeout(getHdl());
}

bool RWHandler::cbMsgDecode(std::vector<MessagePtr>* out)
{
    try {
        m_server.cbMessageDecode(getHdl(), m_read_buffer, out);
        return true;
    } catch (const std::exception& e) {
        LOG(DEBUG) << "exception " << e.what();
        return false;
    }
}

void RWHandler::cbReceivedMsg(std::vector<MessagePtr> messages)
{
    m_server.cbReceivedMsg(getHdl(), std::move(messages));
}

void RWHandler::sendMessage(MessagePtr msg)
{
    m_io_service.post([this, self = shared_from_this(), msg]()
        {
            bool wait_write = !m_write_buffer.empty();
            m_write_buffer.push_back(std::move(msg));
            if (!wait_write) {
                doWrite();
            }
        });
}

void RWHandler::doWrite()
{
    boost::asio::async_write(m_socket
        , boost::asio::buffer(m_write_buffer.front()->data(), m_write_buffer.front()->size())
        , [this, self = shared_from_this()](boost::system::error_code ec, std::size_t length)
        {
            (void)length;
            if (ec) {
                LOG(DEBUG) << "RWHandlerBase::doWrite error: " << ec.value() << ":" << ec.message();
                onClosed(CLOSED_TYPE::NORMAL);
                return;
            }
            m_write_buffer.pop_front();
            if (!m_write_buffer.empty()) {
                doWrite();
            }
        });
}

void RWHandler::doRead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer{};
    if (m_server.getOption().m_timeout_seconds > 0)
        timer = setTimeoutTimer(m_server.getOption().m_timeout_seconds);

    m_socket.async_read_some(boost::asio::buffer(m_read_fix_buffer),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
        {
            timeoutCancel(timer);
            (void)length;
            if (ec) {
                onClosed(CLOSED_TYPE::NORMAL);
                LOG(DEBUG) << "error: " << ec.value() << " msg: " << ec.message();
                return;
            }
            if (length > 0) {
                m_read_buffer.append(m_read_fix_buffer.data(), length);
                std::vector<MessagePtr> out{};
                if (!cbMsgDecode(&out)) {
                    onClosed(CLOSED_TYPE::NORMAL);
                    return;
                }
                if (!out.empty()) {
                    cbReceivedMsg(std::move(out));
                }
                m_read_fix_buffer.fill(0);
                LOG(DEBUG) << "read len :" << length;
            }
            doRead();
        });
}

boost::asio::ip::tcp::socket& RWHandler::getSocket()
{
    return m_socket;
}

boost::asio::io_service& RWHandler::getIOService()
{
    return m_io_service;
}

void RWHandler::onClosed(CLOSED_TYPE type)
{
    LOG(DEBUG) << "closed type:" << int(type);
    if (m_is_closed.exchange(true))
        return;
    if (type == CLOSED_TYPE::NORMAL) {
        cbClosed();
    } else if (type == CLOSED_TYPE::TIMEOUT) {
        cbTimeout();
    }

    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
    m_server.stopHandler(shared_from_this());
}

void RWHandler::shutdown()
{
    if (m_is_closed)
        return;
    m_io_service.post([self = shared_from_this()]() 
        {
            self->onClosed(CLOSED_TYPE::ACTIVITY);
        });
}

std::shared_ptr<boost::asio::deadline_timer> RWHandler::setTimeoutTimer(size_t seconds)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
    timer->expires_from_now(boost::posix_time::seconds(seconds));
    timer->async_wait([self = shared_from_this()](const boost::system::error_code& ec) {
        if (!ec) {
            self->onClosed(CLOSED_TYPE::TIMEOUT);
        }
    });
    return timer;
}

void RWHandler::timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer)
{
    if (timer) {
        boost::system::error_code ec;
        timer->cancel(ec);
    }
}

ConnectionHdl RWHandler::getHdl()
{
    return ConnectionHdl{shared_from_this()};
}

}
