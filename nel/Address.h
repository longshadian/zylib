#pragma once

#include <vector>
#include <memory>
#include <sstream>

#include "Types.h"

namespace nlnet {

struct CInetAddress
{
    std::string m_ip;
    uint16_t    m_port;

    bool operator==(const CInetAddress& rhs) const
    {
        return m_ip == rhs.m_ip && m_port == rhs.m_port;
    }

    bool operator!=(const CInetAddress& rhs) const
    {
        return !(*this == rhs);
    }

    std::string toString() const
    {
        std::ostringstream ostm{};
        ostm << m_ip << ':' << m_port;
        return ostm.str();
    }
};



struct ServiceAddr
{
    std::string m_service_name;
    ServiceID   m_service_id;
    std::vector<CInetAddress> m_addresses;
};

} // nlnet
