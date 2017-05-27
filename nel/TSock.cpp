#include "TSock.h"

#include <chrono>
#include <thread>
#include <future>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Log.h"

namespace NLNET {

TSock::TSock(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket)
    : m_io_service(io_service)
    , m_socket(std::move(socket))
    , m_write_buffer()
    , m_is_closed(false)
{
}

TSock::~TSock()
{
}

bool TSock::connect(const std::string& ip, int32_t port)
{
    return syncConnect(ip, port);
}

void TSock::start()
{
    doRead();
}

void TSock::sendMsg(CMessage msg)
{
    auto self(shared_from_this());
    m_io_service.post([this, self, msg = std::move(msg)]()
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
        std::string s = (const char*)m_read_buffer.data();

        NetWorkMessage msg{};
        m_received_msg_cb(msg, self->getConnectionHdl());

        doRead();
    });
}

boost::asio::ip::tcp::socket& TSock::getSocket()
{
    return m_socket;
}

boost::asio::io_service& TSock::getIoService()
{
    return m_io_service;
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
        m_closed_cb(getConnectionHdl());
    } else if (type == CLOSED_TYPE::TIMEOUT) {
        m_timeout_cb(getConnectionHdl());
    }
    closeSocket();
}

void TSock::shutdown()
{
    if (m_is_closed)
        return;
    auto self(shared_from_this());
    m_io_service.post([this, self]() 
        {
            self->onClosed(CLOSED_TYPE::ACTIVITY);
        });
}

std::shared_ptr<boost::asio::deadline_timer> TSock::setTimeoutTimer(int seconds)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
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

bool TSock::syncConnect(const std::string& ip, int32_t port)
{
    std::promise<bool> p{};
    auto f = p.get_future();
    std::thread t([this, &p, &ip, & port] {
        try {
            boost::asio::ip::tcp::resolver r(m_io_service);
            boost::asio::connect(getSocket(), r.resolve({ip, std::to_string(port)}));
            p.set_value(true);
        } catch (const std::exception& e) {
            p.set_exception(std::current_exception());
        }
    });

    try {
        auto ret = f.get();
        return ret;
    } catch (const std::exception& e) {
        return false;
    }
}

} // NLNET
