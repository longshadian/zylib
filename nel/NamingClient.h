#pragma once

#include <vector>
#include <string>

#include "Types.h"
#include "zylib/Timer.h"

#include <boost/asio.hpp>

namespace nlnet {

class CInetAddress;
class ServiceAddr;

class NamingClient
{
    enum class STATE : int32_t
    {
        CONNECTING = 0, // 正在连接
        CONNECTED  = 1, // 已经链接上了
        DISCONNECT = 2, // 断开链接
    };

public:
    NamingClient(boost::asio::io_service& io_service);
    ~NamingClient();
    NamingClient(const NamingClient& rhs) = delete;
    NamingClient& operator=(const NamingClient& rhs) = delete;

    bool isConnected() const;
    bool connect(const CInetAddress& addr);
    void update(DiffTime diff_time);

    std::vector<ServiceAddr> getRegisterService();
private:
    bool syncConnect(const std::string& ip, int32_t port);
    bool asyncConnect(const CInetAddress& addr);

    void cbConnectFail();
    void cbConnectSuccess();
    void cbReadMessageFail();

    void doRead();
    void doReadBody();

    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(int seconds);
    void onClosed();

    void closeSocket();
    void shutdown();

private:
    std::vector<ServiceAddr> m_online_service;
    zylib::TimingWheel       m_timer;
    std::mutex               m_mtx;
    STATE                    m_state;
    CInetAddress             m_addr;

    boost::asio::ip::tcp::socket    m_socket;
    std::list<CMessagePtr>          m_write_buffer;
    std::atomic<bool>               m_is_closed;
    int32_t                         m_read_timeout_seconds;
    std::array<uint8_t, 4>          m_read_head;
    std::vector<uint8_t>            m_read_body;
};

} // NLNET
