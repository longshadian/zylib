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
    nlnet::ServiceID    m_sid{};
    std::string         m_sname{};
    nlnet::CInetAddress m_addr{};
    nlnet::ServiceID    m_conn_sid{};
};

struct ServiceStatus
{
    nlnet::ServiceID getServiceID() const { return m_conf->m_sid; }
    std::string      getServiceName() const { return m_conf->m_sname; }
    const nlnet::CInetAddress& getInetAddress() const { return m_conf->m_addr; }

    bool isOnline() const
    {
        return m_conf != nullptr;
    }

    nlnet::TSockPtr     m_conn;
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

    void reqService(nlnet::TSockContext sock, nlnet::CMessage& msg);
    void registerService(nlnet::TSockContext sock, nlnet::CMessage& msg);

    const ServiceConf* findServiceConf(nlnet::ServiceID sid) const;
    ServiceStatus* findServiceStatus(nlnet::ServiceID sid);

    std::unique_ptr<nlnet::UnifiedNetwork> m_network;

    std::unordered_map<nlnet::ServiceID, ServiceStatus>  m_service_status;
    std::unordered_map<nlnet::ServiceID, ServiceConf>    m_service_conf;
private:
    void startConf();
    void broadcastNewService(const ServiceStatus& s_status, const ::google::protobuf::Message& msg) const;
};
