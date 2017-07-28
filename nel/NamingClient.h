#pragma once

#include <vector>
#include <string>

#include "Types.h"

#include <boost/asio.hpp>

namespace nlnet {

class CInetAddress;
class ServiceAddr;

class NamingClient
{
    enum class STATE : int32_t
    {
        CONNECTING = 0, // ��������
        CONNECTED  = 1, // �Ѿ���������
        DISCONNECT = 2, // �Ͽ�����
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

private:
    TSockPtr                 m_sock;
    std::vector<ServiceAddr> m_online_service;
    zylib::TimingWheel       m_timer;
    std::mutex               m_mtx;
    STATE                    m_state;
    CInetAddress             m_addr;
};

} // NLNET
