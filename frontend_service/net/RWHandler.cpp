#include "RWHandler.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "FakeLog.h"
#include "StreamServer.h"
#include "Message.h"

RWHandler::RWHandler(boost::asio::ip::tcp::socket socket, const HandlerOption& opt)
    : m_socket(std::move(socket))
    , m_handler_opt(opt)
    , m_is_closed(false)
    , m_write_buffer()
    , m_read_head()
    , m_read_body()
{
}

RWHandler::~RWHandler()
{
}

void RWHandler::init()
{
    doReadHead();
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

void RWHandler::doReadHead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer{};
    if (m_handler_opt.m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_handler_opt.m_read_timeout_seconds);

    boost::asio::async_read(boost::asio::buffer(m_read_head)
        , std::bind(&RWHandler::doReadBody, shared_from_this(), timer, std::placeholders::_1, std::placeholders::_2));
}

void RWHandler::doReadBody()
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_handler_opt.m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_handler_opt.m_read_timeout_seconds);
    boost::asio::async_read(getSocket(), boost::asio::buffer(m_read_head),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
        {
            timeoutCancel(timer);
            (void)length;
            if (ec) {
                onClosed();
                LOG(WARNING) << "readHead error:" << ec.value() << ":" << ec.message();
                return;
            }
            MsgHead head{};
            std::memcpy(&head, m_read_head.data(), MSG_HEAD_SIZE);
            if (head.m_length <= MSG_HEAD_SIZE || head.m_length > 1024) {
                onClosed();
                LOG(ERROR) << "readHead msg_len " << head.m_length;
                return;
            }

            auto msg = std::make_shared<Message>(); 
            std::memcpy(&msg->m_head, &head, MSG_HEAD_SIZE);
            msg->m_body = std::move(m_read_body);
            msg->m_timestamp = std::chrono::system_clock::now();
            m_stream_server.GetCallback().ReceviedMessage(getHdl(), std::move(msg));
            doReadBody();
        });
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
        m_stream_server.GetCallback().HandlerClosed(getHdl());
    } else if (type == CLOSED_TYPE::TIMEOUT) {
        m_stream_server.GetCallback().HandlerTimeout(getHdl());
    }
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
    m_stream_server.stopHandler(shared_from_this());
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
