#pragma once

#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Types.h"
#include "UnifiedNetwork.h"
#include "TSock.h"

#include "msg.pb.h"


struct ServiceConf
{
    NLNET::ServiceID    m_sid{};
    std::string         m_sname{};
    NLNET::CInetAddress m_addr{};
    NLNET::ServiceID    m_conn_sid{};
};

struct ServiceStatus
{
    NLNET::ServiceID getServiceID() const { return m_conf->m_sid; }
    std::string      getServiceName() const { return m_conf->m_sname; }
    const NLNET::CInetAddress& getInetAddress() const { return m_conf->m_addr; }

    bool isOnline() const
    {
        return m_conf != nullptr;
    }

    NLNET::TSockPtr     m_conn;
    time_t              m_conn_tm;
    time_t              m_disconn_tm;
    time_t              m_ping_tm;

    time_t              m_start_tm;
    const ServiceConf*  m_conf;
};

using ServiceStatusPtr = std::shared_ptr<ServiceStatus>;

class NamingService
{
public:
    NamingService();
    ~NamingService();
    NamingService(const NamingService& rhs) = delete;
    NamingService& operator=(const NamingService& rhs) = delete;
    NamingService(NamingService&& rhs) = delete;
    NamingService& operator=(NamingService&& rhs) = delete;

    bool start();

    void reqService(NLNET::TSockContext sock, NLNET::CMessage& msg);
    void registerService(NLNET::TSockContext sock, NLNET::CMessage& msg);

    const ServiceConf* findServiceConf(NLNET::ServiceID sid) const;
    ServiceStatus* findServiceStatus(NLNET::ServiceID sid);

    std::unique_ptr<NLNET::UnifiedNetwork> m_network;

    std::unordered_map<NLNET::ServiceID, ServiceStatus>  m_service_status;
    std::unordered_map<NLNET::ServiceID, ServiceConf>    m_service_conf;
private:
    void startConf();
    void broadcastNewService(const ServiceStatus& s_status, const ::google::protobuf::Message& msg) const;
};
