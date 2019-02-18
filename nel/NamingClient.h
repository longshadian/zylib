#pragma once

#include <vector>
#include <string>

#include <boost/asio.hpp>

#include "zylib/zylib.h"
#include "Types.h"
#include "Address.h"

namespace nlnet {

class CInetAddress;
class ServiceAddr;

class NamingClient : public std::enable_shared_from_this<NamingClient>
{
    enum { CONNECT_SECONDS = 5 };

    enum STATE
    {
        CONNECTING = 0, // 正在连接
        CONNECTED  = 1, // 已经链接上了
        DISCONNECT = 2, // 断开链接
        SHUTDOWN   = 3, // 不使用
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
    bool asyncConnect(const CInetAddress& addr);

    void cbConnectFail();
    void cbConnectSuccess();
    void cbReadMessageFail();

    void doRead();
    void doReadBody();

    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(int seconds);

    void shutdown();

private:
    boost::asio::io_service& getIOService();
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);

    void cbConnectNameService();
    void startRead();
    void setState(STATE s);
    STATE getState() const;
    void close();

private:
    boost::asio::ip::tcp::socket    m_socket;
    std::vector<ServiceAddr>        m_online_service;
    zylib::TimingWheel              m_timer;
    std::atomic<uint32_t>           m_state;
    CInetAddress                    m_addr;

    std::list<CMessagePtr>          m_write_buffer;
    int32_t                         m_read_timeout_seconds;
    std::array<uint8_t, 4>          m_read_head;
    std::vector<uint8_t>            m_read_body;
};

} // NLNET
