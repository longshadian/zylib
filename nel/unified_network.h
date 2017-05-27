#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>

#include <boost/asio.hpp>

#include "Types.h"

namespace NLNET {

class UnifiedConnection;
class NamingClient;

class UnifiedNetwork
{
public:
	UnifiedNetwork();
	~UnifiedNetwork();

    void stop();
    void waitThreadExit();
    bool init(const std::string& short_name, const CInetAddress& addr);

	void connect();

	void addService(const std::string& name, const std::vector<CInetAddress>& addr, bool auto_retry = true);

	void update(DiffTime diff_time);

	bool send(SID service_id, CMessage msg, AddrID add_id = AddrID_Default);

	//void sendAll(const CMessage& msg);
private:
    CUnifiedConnectionPtr findConnection(const std::string& service_name);
    CUnifiedConnectionPtr findConnection(const SID& service_id);
    int32_t findEndpointIndex(SID service_id, AddrID add_id) const;
private:
    boost::asio::io_service                         m_io_service;
    boost::asio::io_service::work                   m_io_work;
    std::thread                                     m_thread;
	std::unordered_map<SID, CUnifiedConnectionPtr>  m_connections;
    std::vector<SID>                                m_conn_to_reset;
	std::vector<SID>                                m_used_conn;
	std::string     m_self_service_name;
	int16_t         m_self_server_port;
    CUnifiedConnectionPtr m_self_conn;
	bool            m_initialised;
    std::shared_ptr<NamingClient>   m_naming_client;
};

} // NLNET
