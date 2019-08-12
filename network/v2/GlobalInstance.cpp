#include "GlobalInstance.h"

#include "network/Message.h"
#include "network/Event.h"
#include "network/TcpClient.h"
#include "network/TcpServer.h"

GlobalInstance::GlobalInstance()
    : m_accect_func()
    , m_connect_func()
    , m_send_func()
    , m_receive_func()
    , m_network_error_func()
    , m_next_index(1)
    , m_mtx()
    , m_client_map()
    , m_server_map()
{
}

GlobalInstance::~GlobalInstance()
{
}

GlobalInstance* GlobalInstance::Get()
{
    static GlobalInstance instance_{};
    return &instance_;
}

void GlobalInstance::Init()
{
    GlobalInstance::Get();
    // TODO
}

void GlobalInstance::Cleanup() 
{
    // TODO
}

std::int64_t GlobalInstance::CreateServer(std::string host, std::uint16_t port)
{
    auto decoder = std::make_shared<MessageDecoder>();
    auto index = NextIndex();
    auto server_event = std::make_shared<TcpServerEvent>();
    auto server = std::make_shared<TcpServer>(index, server_event, decoder, std::move(host), port);
    AddTcpServer(index, server);
    return index;
}

std::int64_t GlobalInstance::CreateClient(std::string host, std::uint16_t port)
{
    auto decoder = std::make_shared<MessageDecoder>();
    auto index = NextIndex();
    auto client_event = std::make_shared<TcpClientEvent>();
    auto client = std::make_shared<TcpClient>(index, client_event, decoder, std::move(host), port);
    AddTcpClient(index, client);
    return index;
}

bool GlobalInstance::TcpLaunch(std::int64_t index)
{
    auto client = FindTcpClient(index);
    if (client) {
        return client->Start();
    }

    auto server = FindTcpServer(index);
    if (server) {
        return server->Start();
    }
    return false;
}

std::int64_t GlobalInstance::NextIndex()
{
    return m_next_index++;
}

std::shared_ptr<TcpClient> GlobalInstance::FindTcpClient(std::int64_t index)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    auto it = m_client_map.find(index);
    if (it != m_client_map.end())
        return it->second;
    return nullptr;
}

std::shared_ptr<TcpServer> GlobalInstance::FindTcpServer(std::int64_t index)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    auto it = m_server_map.find(index);
    if (it != m_server_map.end())
        return it->second;
    return nullptr;
}

void GlobalInstance::AddTcpClient(std::int64_t index, std::shared_ptr<TcpClient> client)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_client_map[index] = client;
}

void GlobalInstance::AddTcpServer(std::int64_t index, std::shared_ptr<TcpServer> server)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_server_map[index] = server;
}

void GlobalInstance::SetEventFunc(DllEvent& dll_event)
{
    dll_event.m_accect_func = m_accect_func;
    dll_event.m_connect_func = m_connect_func;
    dll_event.m_send_func = m_send_func;
    dll_event.m_receive_func = m_receive_func;
    dll_event.m_network_error_func = m_network_error_func;
}
