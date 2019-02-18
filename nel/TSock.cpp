#include "TSock.h"

#include <chrono>
#include <thread>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Log.h"
#include "UnifiedConnection.h"
#include "CMessage.h"

namespace nlnet {

TSock::TSock(boost::asio::ip::tcp::socket socket)
    : m_socket(std::move(socket))
    , m_write_buffer()
    , m_is_closed(false)
    , m_received_msg_cb()
    , m_closed_cb()
    , m_timeout_cb()
    , m_msg_decoder_cb()
    , m_read_timeout_seconds()
    , m_read_buffer()
    , m_byte_buf()
{
    m_read_buffer.fill(0);
}

TSock::~TSock()
{
}

void TSock::start()
{
    m_is_closed.store(false);
    doRead();
}

bool TSock::isClosed() const
{
    return m_is_closed;
}

bool TSock::sendMsg(CMessagePtr msg)
{
    if (m_is_closed)
        return false;
    auto self(shared_from_this());
    getIoService().post([this, self, msg_ex = std::move(msg)]()
        {
            bool wait_write = !m_write_buffer.empty();
            m_write_buffer.push_back(std::move(msg_ex));
            if (!wait_write) {
                doWrite();
            }
        });
    return true;
}

void TSock::doWrite()
{
    auto self(shared_from_this());
    boost::asio::async_write(m_socket, boost::asio::buffer(m_write_buffer.front()->data(), m_write_buffer.front()->size()),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            (void)length;
            if (ec) {
                NL_LOG(WARNING) << "RWHandlerBase::doWrite error: " << ec.value() << ":" << ec.message();
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
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_read_timeout_seconds);

    getSocket().async_read_some(boost::asio::buffer(m_read_buffer),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
        {
            timeoutCancel(timer);
            if (ec) {
                onClosed();
                NL_LOG(WARNING) << "doRead error " << ec.message();
                return;
            }
            if (length > 0) {
                std::vector<CMessagePtr> out{};
                m_byte_buf.append(m_read_buffer.data(), length);
                m_msg_decoder_cb(m_byte_buf, &out);
                if (!out.empty()) {
                    for (auto& msg : out) {
                        auto net_msg = std::make_shared<NetworkMessage>();
                        net_msg->m_sock_hdl = getSockHdl();
                        net_msg->m_msg = msg;
                        m_received_msg_cb(net_msg);
                    }
                }
                m_read_buffer.fill(0);
                NL_LOG(DEBUG) << "read head len :" << length << " decoder msg:" << out.size();
            }
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
    if (m_is_closed.exchange(true))
        return;
    NL_LOG(DEBUG) << "closed type:" << int(type);
    if (type == CLOSED_TYPE::NORMAL) {
        m_closed_cb(shared_from_this());
    } else if (type == CLOSED_TYPE::TIMEOUT) {
        m_timeout_cb(shared_from_this());
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

void TSock::setReceivedMsgCallback(ReceivedMsg_Callback cb)
{
    m_received_msg_cb = std::move(cb);
}

void TSock::setClosedCallback(Closed_Callback cb)
{
    m_closed_cb = std::move(cb);
}

void TSock::setTimeoutCallback(Timeout_Callback cb)
{
    m_timeout_cb = std::move(cb);
}

void TSock::setMessageDecodeCallback(ByteToMessage_Callback cb)
{
    m_msg_decoder_cb = std::move(cb);
}

TSockHdl TSock::getSockHdl()
{
    return TSockHdl{shared_from_this()};
}

boost::asio::io_service& TSock::getIOService()
{
    return m_socket.get_io_service();
}

} // NLNET
