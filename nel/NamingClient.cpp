#include "NamingClient.h"

#include <future>

#include "Log.h"
#include "CMessage.h"

namespace nlnet {

NamingClient::NamingClient(boost::asio::io_service& io_service)
    : m_socket(io_service)
    , m_online_service()
    , m_timer()
    , m_state()
    , m_addr()
    , m_write_buffer()
    , m_read_timeout_seconds()
    , m_read_head()
    , m_read_body()
{
    setState(STATE::DISCONNECT);
}

NamingClient::~NamingClient()
{
}

bool NamingClient::isConnected() const
{
    return getState() == STATE::DISCONNECT;
}

bool NamingClient::connect(const CInetAddress& addr)
{
    m_addr = addr;
    return asyncConnect(addr);
}

void NamingClient::update(DiffTime diff_time)
{
    auto s = m_state.load();
    if (s == static_cast<uint32_t>(STATE::CONNECTED) || s == static_cast<uint32_t>(STATE::CONNECTING))
        return;
    if (s == static_cast<uint32_t>(STATE::DISCONNECT)) {
        if (m_timer.valid()) {
            m_timer.update(diff_time);
            if (m_timer.passed()) {
                m_timer = zylib::TimingWheel{};
                asyncConnect(m_addr);
            }
        } else {
            m_timer = zylib::TimingWheel{std::chrono::seconds{CONNECT_SECONDS}};
        }
    }
}

std::vector<ServiceAddr> NamingClient::getRegisterService()
{
    // TODO 返回在NS注册的service
    return {};
}

bool NamingClient::asyncConnect(const CInetAddress& addr)
{
    try {
        boost::asio::ip::address baddr{};
        baddr.from_string(addr.m_ip);
        boost::asio::ip::tcp::endpoint ep_pair{ baddr, addr.m_port };
        setState(STATE::CONNECTING);
        m_socket.async_connect(ep_pair,
            [this, self = shared_from_this()](boost::system::error_code ec)
        {
            if (!ec) {
                NL_LOG(DEBUG) << "connect " << m_addr << " success";
                startRead();
                cbConnectNameService();
            } else {
                NL_LOG(WARNING) << "connect " << m_addr << " fail " << ec.message();
            }
        });
        return true;
    } catch (std::exception e) {
        setState(STATE::DISCONNECT);
        NL_LOG(WARNING) << "conn exception " << e.what();
    }
    return false;
}

void NamingClient::cbConnectNameService()
{
    // TODO 链接上了NameService
}

void NamingClient::cbConnectFail()
{
    setState(STATE::DISCONNECT);
}

void NamingClient::cbConnectSuccess()
{
    setState(STATE::CONNECTED);

    // TODO 发送注册信息
}

void NamingClient::cbReadMessageFail()
{

}

void NamingClient::doRead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer{};
    if (m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_read_timeout_seconds);
    boost::asio::async_read(m_socket, boost::asio::buffer(m_read_head),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);

        (void)length;
        if (ec) {
            close();
            NL_LOG(WARNING) << "readHead error:" << ec.value() << ":" << ec.message();
            return;
        }

        int32_t body_len = 0;
        std::memcpy(&body_len, m_read_head.data(), 4);
        if (body_len < 4) {
            cbReadMessageFail();
            close();
            return;
        }
        m_read_head.fill(0);
        if (body_len == 4) {
            doRead();
        } else {
            m_read_body.resize(body_len - 4);
            doReadBody();
        }
    });
}

void NamingClient::doReadBody()
{
    std::shared_ptr<boost::asio::deadline_timer> timer{};
    if (m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_read_timeout_seconds);
    boost::asio::async_read(m_socket, boost::asio::buffer(m_read_head),
        [this, self = shared_from_this(), timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);

        (void)length;
        if (ec) {
            close();
            NL_LOG(WARNING) << "readHead error:" << ec.value() << ":" << ec.message();
            return;
        }

        int32_t body_len = 0;
        std::memcpy(&body_len, m_read_head.data(), 4);
        if (body_len < 4) {
            cbReadMessageFail();
            close();
            return;
        }
        m_read_head.fill(0);
        if (body_len == 4) {
            doRead();
        }
        else {
            m_read_body.resize(body_len - 4);
            doReadBody();
        }
    });
}

std::shared_ptr<boost::asio::deadline_timer> NamingClient::setTimeoutTimer(int seconds)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(getIOService());
    timer->expires_from_now(boost::posix_time::seconds(seconds));

    timer->async_wait([this, self = shared_from_this()](const boost::system::error_code& ec) {
        if (!ec) {
            self->close();
        }
    });
    return timer;
}

void NamingClient::shutdown()
{
    getIOService().post([this, self = shared_from_this()]()
    {
        self->close();
    });
}

boost::asio::io_service& NamingClient::getIOService()
{
    return m_socket.get_io_service();
}

void NamingClient::timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer)
{
    if (timer) {
        boost::system::error_code ec;
        timer->cancel(ec);
    }
}

void NamingClient::startRead()
{
    doRead();
}

void NamingClient::setState(STATE s)
{
    m_state.store(static_cast<uint32_t>(s));
}

NamingClient::STATE NamingClient::getState() const
{
    return static_cast<STATE>(m_state.load());
}

void NamingClient::close()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

} // nlnet
