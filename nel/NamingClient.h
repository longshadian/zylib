#pragma once

#include <vector>
#include <string>

#include "Types.h"

#include <boost/asio.hpp>

namespace NLNET {

class NamingClient
{
public:
    NamingClient(boost::asio::io_service& io_service);
    ~NamingClient();
    NamingClient(const NamingClient& rhs) = delete;
    NamingClient& operator=(const NamingClient& rhs) = delete;

    bool isConnected() const;
    bool connect(const CInetAddress& addr);
    void update();

    std::vector<ServiceAddr> getRegisterService();
private:
    bool syncConnect(const std::string& ip, int32_t port);
private:
    TSockPtr                     m_sock;
    bool                         m_is_connected;
};

} // NLNET
