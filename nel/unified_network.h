#pragma once

#include <vector>
#include <string>

#include <boost/asio.hpp>

namespace NLNET {

class CUnifiedConnection;
class NamingClient;

using TUnifiedNetCallback = void(*)(const std::string &serviceName, TServiceId sid, void *arg);

class CUnifiedNetwork
{
public:
	CUnifiedNetwork();
	~CUnifiedNetwork();

    bool init(const std::string& short_name, const CInetAddress& addr);

	void connect();

	void addService(const std::string& name, const std::vector<CInetAddress>& addr, bool auto_retry = true);

	void update(DiffTime diff_time);

	bool send(TServiceId service_id, const CMessage& msg);

	void sendAll(const CMessage& msg);
private:
    CUnifiedConnectionPtr addNewEndpoint(const std::string& service_name, TcpSocket sock,
        CUnifiedConnection::TState state, bool auto_retry);

    CUnifiedConnectionPtr findConnection(const std::string& service_name);
private:
    boost::asio::io_service         m_io_service;
	std::vector<CUnifiedConnectionPtr> m_connections;
    std::vector<CUnifiedConnectionPtr> m_conn_to_reset;
	std::vector<CUnifiedConnectionPtr> m_used_conn;

    std::unordered_multimap<std::string, TServiceId> m_conn_service_names;
	std::string     m_self_service_name;
	uint16_t        m_self_server_port;
    CUnifiedConnectionPtr m_self_conn;
	bool            m_initialised;
    std::unique_ptr<NamingClient>   m_naming_client;
};

} // NLNET
