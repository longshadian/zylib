#include "unified_network.h"

#include "Log.h"

#include "unified_connection.h"

using namespace std;

namespace NLNET {

CUnifiedNetwork::CUnifiedNetwork()
{

}

CUnifiedNetwork::~CUnifiedNetwork()
{

}

bool CUnifiedNetwork::init(const std::string& short_name, const CInetAddress& addr)
{
    if (m_initialised)
        return false;

    m_self_server_port = port;
    m_self_service_name = short_name;

    // 先监听自己
    try {
        TcpSocket sock{};
        boost::asio::ip::tcp::resolver r(m_io_service);
        boost::asio::connect(sock, r.resolve({ addr.m_ip, addr.m_port }));
        m_self_conn = addNewEndpoint(m_self_service_name, std::move(sock), CUnifiedConnection::TState::Ready, true);
    } catch (const std::exception& e) {
        LOG_WARNING() << "connect to naming service failed:" << e.what();
        return false;
    }

    // 连接naming服务器
    CInetAddress naming_addr{};
    if (!m_naming_client->connect(naming_addr)) {
        LOG_WARNING() << "CUnifiedNetwork init failed: naming service connection failed"; 
        return false;
    }

    // TODO 从NS拉取已经注册的服务器信息
}

void CUnifiedNetwork::connect()
{
    // 注册到NS服务器,并且注册到所有已经启动的服务
    auto registered_service = m_naming_client->getRegisterService();
    for (const auto& service : registered_service) {
        // 不要自己连自己
        if (service.m_service_name == m_self_service_name) {
            continue;
        }

        addService(service.m_service_name, service.m_addresses);
    }
}

void CUnifiedNetwork::addService(const std::string& name, const std::vector<CInetAddress>& addr, bool auto_retry)
{
    auto conn = findConnection(name);
    if (!conn) {
        conn = std::make_shared<CUnifiedConnection>();
        conn->m_ServiceName = name;
        conn->m_AutoRetry = auto_retry;
    }

    bool	connectSuccess;
    for (size_t i = 0; i != addr.size(); ++i) {

        try {
            cbc->connect(addr[i]);
            connectSuccess = true;
        } catch (const ESocketConnectionFailed &e) {
            nlwarning("HNETL5: can't connect to %s (sid %u) now (%s) '%s'", name.c_str(), sid.get(), e.what(), addr[i].asString().c_str());
            connectSuccess = false;
        }

        if (!connectSuccess && !autoRetry)
        {
            nlwarning("HNETL5: Can't add service because no retry and can't connect");
            delete cbc;
        }
        else
        {
            uc->Connections[i] = CUnifiedNetwork::CUnifiedConnection::TConnection(cbc);
        }

        if (connectSuccess && sendId)
        {
            // send identification to the service
            CMessage	msg("UN_SIDENT");
            msg.serial(_Name);
            TServiceId		ssid = _SId;
            if (uc->IsExternal)
            {
                // in the case that the service is external, we can't send our sid because the external service can
                // have other connection with the same sid (for example, LS can have 2 WS with same sid => sid = 0 and leave
                // the other side to find a good number
                ssid.set(0);
            }
            msg.serial(ssid);	// serializes a 16 bits service id
            uint8 pos = uint8(j);
            msg.serial(pos);	// send the position in the connection table
            msg.serial(uc->IsExternal);
            cbc->send(msg);
        }
    }

    if (addr.size() != uc->Connections.size())
    {
        nlwarning("HNETL5: Can't connect to all connections to the service %d/%d", addr.size(), uc->Connections.size());
    }

    bool cntok = false;
    for (uint j = 0; j < uc->Connections.size(); j++)
    {
        if (uc->Connections[j].CbNetBase != NULL)
        {
            if (uc->Connections[j].CbNetBase->connected())
            {
                cntok = true;
                break;
            }
        }
    }

    if (cntok)
    {
        // add the name only if at least one connection is ok
        addNamedCnx(name, sid);

        callServiceUpCallback(name, sid); // global callback ("*") will be called even for external service
    }
    nldebug("HNETL5: addService was successful");
}

void CUnifiedNetwork::update(DiffTime diff_time)
{

}

bool CUnifiedNetwork::send(TServiceId service_id, const CMessage& msg)
{

}

void CUnifiedNetwork::sendAll(const CMessage& msg)
{

}

CUnifiedConnectionPtr CUnifiedNetwork::addNewEndpoint(const std::string& service_name, TcpSocket sock,
    CUnifiedConnection::TState state, bool auto_retry)
{
    auto conn = findConnection(service_name);
    if (conn) {
        conn->addEndpoint(false, std::move(sock));
        return conn;
    } else {
        conn = std::make_shared<CUnifiedConnection>();
        conn->m_ServiceName = service_name;
        conn->m_State = CUnifiedConnection::TState::Ready;
        conn->m_AutoRetry = auto_retry;
        conn->addEndpoint(false, std::move(sock));
        m_connections.push_back(conn);
        return conn;
    }
}

CUnifiedConnectionPtr CUnifiedNetwork::findConnection(const std::string& service_name)
{
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
        [](const CUnifiedConnection& conn)
        { 
            return conn.m_ServiceName = service_name;
        });
    if (it != m_connections.end()) {
        return *it;
    }
    return nullptr;
}


} // NLNET
