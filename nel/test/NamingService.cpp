#include "NamingService.h"

#include <ctime>
#include <iostream>
#include <string>
#include <chrono>

#include "Log.h"

#include "UnifiedNetwork.h"
#include "UnifiedConnection.h"
#include "Tools.h"

#include "CallbackManager.h"
#include "TSock.h"
#include "TestDefine.h"

#define PARSE_REQ(req, msg)\
do { \
    if (!req.ParseFromArray(msg.data(), static_cast<int>(msg.size()))) { \
        std::cout << "parse req " << req.GetTypeName() << "\n"; \
        return; \
    } \
} while(0)

void NamingService::reqService(NLNET::TSockContext sock, NLNET::CMessage& msg)
{
    (void)sock;
    ::pt::req_service req{};
    PARSE_REQ(req, msg);
    std::cout << "req " << req.sid() << "\n";
    ::pt::rsp_service rsp{};
    (void)rsp;
}

void NamingService::registerService(NLNET::TSockContext sock, NLNET::CMessage& msg)
{
    (void)sock;
    ::pt::req_register_service req{};
    PARSE_REQ(req, msg);
    std::cout << "req " << req.sid() << "\n";
    ::pt::rsp_service rsp{};

    auto* s_status = findServiceStatus(req.sid());
    if (!s_status) {
        std::cout << "unknown service sid:" << req.sid() << "\n";
        return;
    }
    if (s_status->isOnline()) {
        std::cout << "service already online sid:" << req.sid() << " sname:" << s_status->getServiceName() << "\n";
        return;
    }

    s_status->m_conn = sock.m_sock;
    s_status->m_conn_tm = std::time(nullptr);

    // 返回注册成功
    {
        ::pt::rsp_register_service rsp_msg{};
        rsp_msg.set_sid(s_status->getServiceID());
    }

    // 广播新注册的service
    {
        ::pt::broadcast_register_service proto_msg{};
        auto* p_s = proto_msg.mutable_service();
        p_s->set_sid(s_status->getServiceID());
        p_s->set_sname(s_status->getServiceName());
        auto* addr = p_s->add_addrs();
        addr->set_ip(s_status->getInetAddress().m_ip);
        addr->set_port(s_status->getInetAddress().m_port);
        broadcastNewService(*s_status, proto_msg);
    }
}

NamingService::NamingService()
{
}

NamingService::~NamingService()
{
}

bool NamingService::start()
{
    m_network = std::make_unique<NLNET::UnifiedNetwork>();

    NLNET::MsgCallbackArray call_array =
    {
        {"_req_service", std::bind(&NamingService::reqService, this, std::placeholders::_1, std::placeholders::_2)},
        {"req_register_service", std::bind(&NamingService::registerService, this, std::placeholders::_1, std::placeholders::_2)},
    };
    m_network->getCallbackManager().setMsgCallbackArray(std::move(call_array));
    if (!m_network->init(def::GS_SID, def::GS_NAME, def::GS_ADDR)) {
        std::cout << "network init faild!\n";
        return false;
    }
    return true;
}

const ServiceConf* NamingService::findServiceConf(NLNET::ServiceID sid) const
{
    auto it = m_service_conf.find(sid);
    if (it != m_service_conf.end())
        return &it->second;
    return nullptr;
}

ServiceStatus* NamingService::findServiceStatus(NLNET::ServiceID sid)
{
    auto it = m_service_status.find(sid);
    if (it != m_service_status.end())
        return &it->second;
    return nullptr;
}

void NamingService::startConf()
{
    ServiceConf gs_conf{};
    gs_conf.m_sid = def::GS_SID;
    gs_conf.m_sname = def::GS_NAME;
    gs_conf.m_addr = def::GS_ADDR;
    gs_conf.m_conn_sid = def::DBS_SID;
    m_service_conf[gs_conf.m_sid] = gs_conf;

    ServiceConf dbs_conf{};
    dbs_conf.m_sid = def::DBS_SID;
    dbs_conf.m_sname = def::DBS_NAME;
    dbs_conf.m_addr = def::DBS_ADDR;
    m_service_conf[dbs_conf.m_sid] = dbs_conf;

    for (const auto& it : m_service_conf) {
        ServiceStatus service_status{};
        service_status.m_conf = &it.second;
        m_service_status[service_status.getServiceID()] = service_status;
    }
}

void NamingService::broadcastNewService(const ServiceStatus& s_status
    , const ::google::protobuf::Message& msg) const
{
    NLNET::CMessage bs_msg{};
    NLNET::tools::protoToCMessage(msg, &bs_msg);

    for (const auto& it : m_service_status) {
        if (it.second.getServiceID() == s_status.getServiceID())
            continue;
        if (!it.second.isOnline())
            continue;
        it.second.m_conn->sendMsg(bs_msg);
    }
}

int main()
{
    NamingService ns{};
    ns.start();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "out\n";
    return 0;
}
