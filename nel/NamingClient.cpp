#include "NamingClient.h"

#include <future>

#include "Log.h"
#include "TSock.h"
#include "CMessage.h"
#include "Address.h"

namespace nlnet {

NamingClient::NamingClient(boost::asio::io_service& io_service)
    : m_sock(std::make_shared<TSock>(boost::asio::ip::tcp::socket(io_service)))
    , m_online_service()
    , m_timer()
    , m_mtx()
    , m_state(STATE::DISCONNECT)
    , m_addr()
{
}

NamingClient::~NamingClient()
{
}

bool NamingClient::isConnected() const
{
    return m_sock->isClosed();
}

bool NamingClient::connect(const CInetAddress& addr)
{
    m_addr = addr;
    m_is_connected = syncConnect(addr.m_ip, addr.m_port);
    return m_is_connected;
}

void NamingClient::update(DiffTime diff_time)
{
    STATE s = STATE::DISCONNECT;
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        s = m_state;
    }
    if (s == STATE::CONNECTED || s == STATE::CONNECTING)
        return;
    if (s == STATE::DISCONNECT) {
        if (m_timer.valid()) {
            m_timer.update(diff_time);
            if (m_timer.passed()) {
                m_timer = zylib::TimingWheel{};
                {
                    std::lock_guard<std::mutex> lk{ m_mtx };
                    m_state = STATE::CONNECTING;
                }
                asyncConnect(m_addr);
            }
        } else {
            m_timer = zylib::TimingWheel{ std::chrono::seconds{10} };
        }
    }
}

std::vector<ServiceAddr> NamingClient::getRegisterService()
{
    // TODO 返回在NS注册的service
    return {};
}

bool NamingClient::syncConnect(const std::string& ip, int32_t port)
{
    std::promise<bool> p{};
    auto f = p.get_future();
    std::thread t([this, &p, &ip, &port] {
        try {
            boost::asio::ip::tcp::resolver r(m_sock->getIoService());
            boost::asio::connect(m_sock->getSocket(),
                r.resolve({ ip, std::to_string(port) }));
            p.set_value(true);
            m_sock->start();
        } catch (std::exception e) {
            NL_LOG(WARNING) << "conn exception " << e.what();
            p.set_value(false);
        }
    });
    t.join();
    try {
        auto ret = f.get();
        return ret;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NamingClient::asyncConnect(const CInetAddress& addr)
{
    try {
        boost::asio::ip::address baddr{};
        baddr.from_string(addr.m_ip);
        boost::asio::ip::tcp::endpoint ep_pair{ baddr, addr.m_port };
        m_state = IS_CONNECTING;
        m_sock->getSocket().async_connect(ep_pair,
            [this, self = shared_from_this()](boost::system::error_code ec)
        {
            if (!ec) {
                NL_LOG(DEBUG) << "connect " << m_inet_addr.toString() << " success";
                startRead();
                cbConnectNameService();
            } else {
                NL_LOG(WARNING) << "connect " << m_inet_addr.toString() << " fail " << ec.message();
            }
        });
        return true;
    } catch (std::exception e) {
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
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_state = STATE::DISCONNECT;
}

void NamingClient::cbConnectSuccess()
{
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        m_state = STATE::CONNECTED;
    }

    // TODO 发送注册信息
}

void NamingClient::cbReadMessageFail()
{

}

void NamingClient::doRead()
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    if (m_read_timeout_seconds > 0)
        timer = setTimeoutTimer(m_read_timeout_seconds);
    auto self(shared_from_this());
    boost::asio::async_read(m_socket, boost::asio::buffer(m_read_head),
        [this, self, timer](boost::system::error_code ec, size_t length)
    {
        timeoutCancel(timer);

        (void)length;
        if (ec) {
            onClosed();
            NL_LOG(WARNING) << "readHead error:" << ec.value() << ":" << ec.message();
            return;
        }

        int32_t body_len = 0;
        std::memcpy(&body_len, m_read_head.data(), 4);
        if (body_len < 4) {
            cbReadMessageFail();
            onClosed();
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

std::shared_ptr<boost::asio::deadline_timer> NamingClient::setTimeoutTimer(int seconds)
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

void NamingClient::onClosed()
{
    if (m_is_closed.exchange(true))
        return;
    closeSocket();
}

void NamingClient::closeSocket()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

void NamingClient::shutdown()
{
    if (m_is_closed)
        return;
    auto self(shared_from_this());
    getIoService().post([this, self]()
    {
        self->onClosed();
    });
}

} // nlnet
