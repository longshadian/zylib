#include "RWHandler.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "FakeLog.h"
#include "StreamServer.h"
#include "Message.h"

namespace network {

RWHandler::RWHandler(boost::asio::ip::tcp::socket socket, const HandlerOption& opt)
    : m_socket(std::move(socket))
    , m_handler_opt(opt)
    , m_is_closed(false)
    , m_write_buffer()
    , m_read_fix_buffer()
    , m_read_buffer()
    , m_async_closed()
    , m_async_timeout()
    , m_async_receive_mgs()
    , m_async_decode()
{
}

RWHandler::~RWHandler()
{
}

void RWHandler::init()
{
    doRead();
}

bool RWHandler::asyncMsgDecode(std::vector<MessagePtr>* out)
{
    try {
        if (m_async_decode)
            m_async_decode(getHdl(), m_read_buffer, out);
        return true;
    } catch (const std::exception& e) {
        LOG(DEBUG) << "exception " << e.what();
        return false;
    }
}

void RWHandler::sendMessage(MessagePtr msg)
{
    getIOService().post([this, self = shared_from_this(), msg]()
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
        , std::bind(&RWHandler::doWriteCallback, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void RWHandler::doWriteCallback(boost::system::error_code ec, std::size_t length)
{
    (void)length;
    if (ec) {
        LOG(DEBUG) << " error: " << ec.value() << ":" << ec.message();
        doClosed(CLOSED_TYPE::NORMAL);
        return;
    }
    m_write_buffer.pop_front();
    if (!m_write_buffer.empty()) {
        doWrite();
    }
}

void RWHandler::doRead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer{};
    if (m_handler_opt.m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_handler_opt.m_read_timeout_seconds);

    m_socket.async_read_some(boost::asio::buffer(m_read_fix_buffer)
        , std::bind(&RWHandler::doReadCallback, shared_from_this(), timer, std::placeholders::_1, std::placeholders::_2));
}

void RWHandler::doReadCallback(DeadlineTimerPtr timer, boost::system::error_code ec, std::size_t length)
{
    timeoutCancel(timer);
    (void)length;
    if (ec) {
        doClosed(CLOSED_TYPE::NORMAL);
        LOG(DEBUG) << "error: " << ec.value() << " msg: " << ec.message();
        return;
    }
    if (length > 0) {
        m_read_buffer.append(m_read_fix_buffer.data(), length);
        std::vector<MessagePtr> msg_list{};
        if (!asyncMsgDecode(&msg_list)) {
            doClosed(CLOSED_TYPE::NORMAL);
            return;
        }
        if (!msg_list.empty()) {
            if (m_async_receive_mgs)
                m_async_receive_mgs(getHdl(), std::move(msg_list));
        }
        m_read_fix_buffer.fill(0);
        LOG(DEBUG) << "read len :" << length;
        m_read_buffer.shrinkToFit();
    }
    doRead();
}

boost::asio::ip::tcp::socket& RWHandler::getSocket()
{
    return m_socket;
}

void RWHandler::doClosed(CLOSED_TYPE type)
{
    LOG(DEBUG) << "closed type:" << int(type);
    if (m_is_closed.exchange(true))
        return;
    if (type == CLOSED_TYPE::NORMAL) {
        if (m_async_closed)
            m_async_closed(getHdl());
    } else if (type == CLOSED_TYPE::TIMEOUT) {
        if (m_async_timeout)
            m_async_timeout(getHdl());
    }
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

void RWHandler::shutdown()
{
    if (m_is_closed)
        return;
    getIOService().post([self = shared_from_this()]() 
        {
            self->doClosed(CLOSED_TYPE::ACTIVITY);
        });
}

RWHandler::DeadlineTimerPtr RWHandler::setTimeoutTimer(size_t seconds)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(getIOService());
    timer->expires_from_now(boost::posix_time::seconds(seconds));
    timer->async_wait([self = shared_from_this()](const boost::system::error_code& ec) {
        if (!ec) {
            self->doClosed(CLOSED_TYPE::TIMEOUT);
        }
    });
    return timer;
}

void RWHandler::timeoutCancel(DeadlineTimerPtr timer)
{
    if (timer) {
        boost::system::error_code ec;
        timer->cancel(ec);
    }
}

boost::asio::io_service& RWHandler::getIOService()
{
    return m_socket.get_io_service();
}

Hdl RWHandler::getHdl()
{
    return Hdl{shared_from_this()};
}

void RWHandler::setCB_AsyncReceiveMsg(CB_ReceivedMessage cb)
{
    m_async_receive_mgs = std::move(cb);
}

void RWHandler::setCB_AsyncTimeout(CB_HandlerTimeout cb)
{
    m_async_timeout = std::move(cb);
}

void RWHandler::setCB_AsyncClosed(CB_HandlerClosed cb)
{
    m_async_closed = std::move(cb);
}

void RWHandler::setCB_AsyncDecode(CB_MessageDecoder cb)
{
    m_async_decode = std::move(cb);
}

}
