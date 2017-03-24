#include "RWHandler.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "FakeLog.h"
#include "MsgDefine.h"
#include "Message.h"

#include "ZYCrypt.h"
#include "WorldManager.h"

RWHandler::RWHandler(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket, int timeout)
    : m_io_service(io_service)
    , m_socket(std::move(socket))
    , m_callback_on_closed()
    , m_write_buffer()
    , m_read_head()
    , m_read_body()
    , m_head_length(0)
    , m_head_key(0)
    , m_timeout(timeout)
    , m_is_closed(false)
{
}

void RWHandler::start()
{
    onAccept();
    doReadHead();
}

void RWHandler::doReadHead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_timeout > 0)
        timer = setTimeoutTimer(m_timeout);
    auto self(shared_from_this());
    boost::asio::async_read(m_socket, boost::asio::buffer(m_read_head),
        [this, self, timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);

        (void)length;
        if (ec) {
            onClosed();
            FAKE_LOG_ERROR() << "RWHandler::doReadHead error " << ec.message();
            return;
        }

        m_head_length = 0;
        m_head_key = 0;
        memcpy(&m_head_length, m_read_head.data(), 4);
        memcpy(&m_head_key, m_read_head.data() + 4, 4);

        if (m_head_length < 16) {
            onClosed();
            FAKE_LOG_ERROR() << "RWHandler::doReadHead msg_len < 16";
            return;
        }

        if (m_head_length > msgdef::MSG_MAX_LEN) {
            onClosed();
            FAKE_LOG_ERROR() << "RWHandler::doReadHead  msg_len > " << msgdef::MSG_MAX_LEN;
            return;
        }
        m_read_body.resize(m_head_length - 8);
        doReadBody();
    });
}

void RWHandler::doReadBody()
{
    /*
     *  |---------HEAD-------|--------BODY--------------|
     *   head_len   head_key   msg_id   msg    body_key
     *     4        4           4       ?      4
     */

    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_timeout > 0)
        timer = setTimeoutTimer(m_timeout);
    auto self(shared_from_this());
    boost::asio::async_read(m_socket, boost::asio::buffer(m_read_body),
        [this, self, timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);
        (void)length;
        if (ec) {
            onClosed();
            FAKE_LOG_ERROR() << "RWHandler::doReadBody doReadBody error " << ec.message();
            return;
        }

        //Ω‚√‹
        share::decrypt(m_read_body.data(), static_cast<int>(m_read_body.size()), m_head_key,  m_read_body.data());

        const auto* pos = m_read_body.data();
        int32_t msg_id = 0;
        ::memcpy(&msg_id, pos, sizeof(msg_id));
        if (!(0 < msg_id && msg_id < msgdef::MSGID_COUNT)) {
            onClosed();
            FAKE_LOG_ERROR() << "RWHandler::doReadBody MessageDecoder error msg_id error msgid:" << msg_id;
            return;
        }
        pos += sizeof(msg_id);

        uint32_t body_key = 0;
        const auto* pos_end = m_read_body.data() + m_read_body.size();
        pos_end -= sizeof(body_key);
        ::memcpy(&body_key, pos_end, sizeof(body_key));
        if (m_head_key != body_key) {
            onClosed();
            FAKE_LOG_ERROR() << "RWHandler::doReadBody MessageDecoder error head key";
            return;
        }

        auto msg = std::make_shared<Message>();
        msg->setMsgData(pos);
        msg->setMsgLength(m_head_length - 4 - 4 - 4 - 4);
        msg->m_timestamp   = std::chrono::system_clock::now();
        msg->m_head_len    = m_head_length;
        msg->m_key         = m_head_key;
        msg->m_msg_id      = msg_id;
        msg->m_body_data   = std::move(m_read_body);

        onReceivedMsg(std::move(msg));

        m_head_length = 0;
        m_head_key = 0;
        m_read_body.clear();
        doReadHead();
    });
}

void RWHandler::sendMsg(int32_t msg_id, const ::google::protobuf::Message* msg)
{
    /*
     *  |---------HEAD-------|--------BODY--------------|
     *   head_len   head_key   msg_id   msg    body_key
     *     4        4           4       ?      4
     */

    int32_t msg_len = static_cast<int32_t>(msg->ByteSize());
    int32_t body_len = 4 + msg_len + 4;
    int32_t head_len = 4 + 4 + body_len;
    uint32_t key = share::cryptKey();

    std::vector<uint8_t> data{};
    data.resize(head_len);

    auto pos = data.data();
    ::memcpy(pos, &head_len, sizeof(head_len));
    pos += sizeof(head_len);

    ::memcpy(pos, &key, sizeof(key));
    pos += sizeof(key);


    auto start_encrypt = pos;
    ::memcpy(pos, &msg_id, sizeof(msg_id));
    pos += sizeof(msg_id);

    if (!msg->SerializeToArray(pos, static_cast<int>(msg_len))) {
        return;
    }
    pos += msg_len;

    ::memcpy(pos, &key, sizeof(key));

    //∂‘bodyº”√‹
    share::encrypt(start_encrypt, body_len, key, start_encrypt);
    appendMsg(std::move(data));
}

void RWHandler::appendMsg(std::vector<uint8_t> msg)
{
    auto self(shared_from_this());
    m_io_service.post([this, self, msg]()
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
    auto self(shared_from_this());
    boost::asio::async_write(m_socket, boost::asio::buffer(m_write_buffer.front().data(), m_write_buffer.front().size()),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            (void)length;
            if (ec) {
                FAKE_LOG_ERROR() << "RWHandler::doWrite error " << ec.message();
                onClosed();
                return;
            }
            m_write_buffer.pop_front();
            if (!m_write_buffer.empty()) {
                doWrite();
            }
        });
}

boost::asio::ip::tcp::socket& RWHandler::getSocket()
{
    return m_socket;
}

void RWHandler::closeSocket()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);

    if (m_callback_on_closed)
        m_callback_on_closed(shared_from_this());
}

void RWHandler::setCallbackOnClosed(std::function<void(const RWHandlerPtr& conn)> f)
{
    m_callback_on_closed = std::move(f);
}

void RWHandler::onAccept()
{
    g_world_mgr.networkAccept(shared_from_this());
}

void RWHandler::onReceivedMsg(MessagePtr msg)
{
    g_world_mgr.networkReceviedMsg(shared_from_this(), std::move(msg));
}

void RWHandler::onClosed(CLOSED_TYPE type)
{
    FAKE_LOG_DEBUG() << " close type:" << int(type);
    if (m_is_closed.exchange(true))
        return;

    if (type == CLOSED_TYPE::NORMAL_CLOSED) {
        g_world_mgr.networkClosed(shared_from_this());
    } else if (type == CLOSED_TYPE::TIMEOUT_CLOSED) {
        g_world_mgr.networkTimeout(shared_from_this());
    }
    closeSocket();
}

void RWHandler::shutdownSocket()
{
    if (m_is_closed)
        return;
    auto self(shared_from_this());
    m_io_service.post([this, self]() 
        {
            self->onClosed(CLOSED_TYPE::ACTIVITY_CLOSED);
        });
}

std::shared_ptr<boost::asio::deadline_timer> RWHandler::setTimeoutTimer(int seconds)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
    timer->expires_from_now(boost::posix_time::seconds(seconds));

    auto self(shared_from_this());
    timer->async_wait([self](const boost::system::error_code& ec) {
        if (!ec) {
            self->onClosed(CLOSED_TYPE::TIMEOUT_CLOSED);
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
