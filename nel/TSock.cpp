#include "TSock.h"

#include <chrono>
#include <thread>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Log.h"
#include "UnifiedConnection.h"

namespace NLNET {

TSock::TSock(boost::asio::ip::tcp::socket socket, UnifiedConnection& conn)
    : m_conn(conn)
    , m_socket(std::move(socket))
    , m_write_buffer()
    , m_is_closed(false)
    , m_received_msg_cb()
    , m_closed_cb()
    , m_timeout_cb()
    , m_read_timeout_seconds()
    , m_read_head()
    , m_read_buffer()
    , m_sock_id(-1)
{
    m_read_head.fill(0);
}

TSock::~TSock()
{
}

void TSock::start()
{
    doRead();
}

void TSock::sendMsg(CMessage msg)
{
    std::vector<uint8_t> all{};
    auto body = msg.serializeToArray();
    all.resize(4 + body.size());

    int32_t len = (int32_t)body.size() + 4;
    std::memcpy(all.data(), &len, 4);
    std::memcpy(all.data() + 4, body.data(), body.size());

    auto self(shared_from_this());
    getIoService().post([this, self, msg = std::move(all)]()
        {
            bool wait_write = !m_write_buffer.empty();
            m_write_buffer.push_back(std::move(msg));
            if (!wait_write) {
                doWrite();
            }
        });
}

void TSock::doWrite()
{
    auto self(shared_from_this());
    boost::asio::async_write(m_socket, boost::asio::buffer(m_write_buffer.front().data(), m_write_buffer.front().size()),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            (void)length;
            if (ec) {
                LOG_WARNING << "RWHandlerBase::doWrite error: " << ec.value() << ":" << ec.message();
                onClosed();
                return;
            }
            m_write_buffer.pop_front();
            if (!m_write_buffer.empty()) {
                doWrite();
            }
        });
}

void TSock::doRead()
{
    // TODO ÔõÃ´¶Á°ü
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_read_timeout_seconds);
    boost::asio::async_read(getSocket(), boost::asio::buffer(m_read_head),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);
        (void)length;
        if (ec) {
            onClosed();
            LOG_WARNING << "readBody error " << ec.message();
            return;
        }

        int32_t len = 0;
        std::memcpy(&len, m_read_head.data(), m_read_head.size());
        m_read_buffer.resize(len - 4);

        LOG_DEBUG << "read head len :" << len;
        doReadBody();
    });
}

void TSock::doReadBody()
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_read_timeout_seconds);
    boost::asio::async_read(getSocket(), boost::asio::buffer(m_read_buffer),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);
        (void)length;
        if (ec) {
            onClosed();
            LOG_WARNING << "readBody error " << ec.message();
            return;
        }

        auto msg = std::make_shared<NetWorkMessage>();
        msg->m_sock_id = getSockID();
        msg->m_msg.parseFromArray(m_read_buffer);
        //m_received_msg_cb(msg, self->getConnectionHdl());

        LOG_DEBUG << "read body: " <<  msg->m_msg.getMsgName() 
            << " data:" << msg->m_msg.getData();
        m_conn.onReceivedMsg(std::move(msg));

        m_read_head.fill(0);
        m_read_buffer.clear();

        doRead();
    });
}

boost::asio::ip::tcp::socket& TSock::getSocket()
{
    return m_socket;
}

boost::asio::io_service& TSock::getIoService()
{
    return m_socket.get_io_service();
}

void TSock::closeSocket()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

void TSock::onClosed(CLOSED_TYPE type)
{
    LOG_DEBUG << "closed type:" << int(type);
    if (m_is_closed.exchange(true))
        return;
    if (type == CLOSED_TYPE::NORMAL) {
        //m_closed_cb(getConnectionHdl());
        m_conn.onServerSockClosed(getSockID());
    } else if (type == CLOSED_TYPE::TIMEOUT) {
        //m_timeout_cb(getConnectionHdl());
        m_conn.onServerSockTimeout(getSockID());
    }
    closeSocket();
}

void TSock::shutdown()
{
    if (m_is_closed)
        return;
    auto self(shared_from_this());
    getIoService().post([this, self]() 
        {
            self->onClosed(CLOSED_TYPE::ACTIVITY);
        });
}

std::shared_ptr<boost::asio::deadline_timer> TSock::setTimeoutTimer(int seconds)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(getIoService());
    timer->expires_from_now(boost::posix_time::seconds(seconds));

    auto self(shared_from_this());
    timer->async_wait([self](const boost::system::error_code& ec) {
        if (!ec) {
            self->onClosed(CLOSED_TYPE::TIMEOUT);
        }
    });
    return timer;
}

void TSock::timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer)
{
    if (timer) {
        boost::system::error_code ec;
        timer->cancel(ec);
    }
}

TSockHdl TSock::getConnectionHdl()
{
    return TSockHdl{shared_from_this()};
}

void TSock::setReceivedMsgCB(ReceivedMsgCallback cb)
{
    m_received_msg_cb = std::move(cb);
}

SockID TSock::getSockID() const
{
    return m_sock_id;
}

void TSock::setSockID(SockID sid)
{
    m_sock_id = sid;
}

boost::asio::io_service& TSock::getIOService()
{
    return m_socket.get_io_service();
}

} // NLNET
