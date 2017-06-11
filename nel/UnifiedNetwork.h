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
class CallbackManager;

class UnifiedNetwork
{
public:
	UnifiedNetwork();
	~UnifiedNetwork();

    void stop();
    void waitThreadExit();
    bool init(ServiceID service_id, const std::string& short_name, const CInetAddress& addr, bool use_ns = false);

	void connect();

	void addService(ServiceID service_id, const std::string& name, const CInetAddress& addr, bool auto_retry = false);
	void addService(ServiceID service_id, const std::string& name, const std::vector<CInetAddress>& addr, bool auto_retry = false);

	void update(DiffTime diff_time);
	bool send(ServiceID service_id, CMessage msg, const CInetAddress& addr);

	//void sendAll(const CMessage& msg);

    CallbackManager& getCallbackManager();
private:
    UnifiedConnectionPtr findConnection(const std::string& service_name);
    UnifiedConnectionPtr findConnection(const ServiceID& service_id);
    void addConnection(UnifiedConnectionPtr conn);
private:
    boost::asio::io_service                         m_io_service;
    boost::asio::io_service::work                   m_io_work;
    std::thread                                     m_thread;
	std::unordered_map<ServiceID, UnifiedConnectionPtr>  m_connections;
    std::vector<ServiceID>                                m_conn_to_reset;
	std::vector<ServiceID>                                m_used_conn;
	std::string                                     m_self_service_name;
	int16_t                                         m_self_server_port;
    UnifiedConnectionPtr                           m_self_conn;
	bool                                            m_initialised;
    std::shared_ptr<NamingClient>                   m_naming_client;
    std::shared_ptr<CallbackManager>                m_cb_manager;
    bool                                            m_use_naming_service;
};

} // NLNET
