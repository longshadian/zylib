#pragma once

#include <cstdint>
#include <atomic>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "DllDefine.h"

class TcpClient;
class TcpServer;
class MessageDecoder;
class NetworkEvent;
class DllEvent;

class GlobalInstance
{
    GlobalInstance();
public:
    ~GlobalInstance();

    static GlobalInstance* Get();
    void Init();
    void Cleanup();

    std::int64_t CreateServer(std::string host, std::uint16_t port);
    std::int64_t CreateClient(std::string host, std::uint16_t port);
    bool TcpLaunch(std::int64_t index);

    std::int64_t NextIndex();

    AcceptFunc                      m_accect_func;
    ConnectFunc                     m_connect_func;
    SendFunc                        m_send_func;
    ReceiveFunc                     m_receive_func;
    NetworkErrorFunc                m_network_error_func;

private:
    std::shared_ptr<TcpClient> FindTcpClient(std::int64_t index);
    std::shared_ptr<TcpServer> FindTcpServer(std::int64_t index);
    void AddTcpClient(std::int64_t index, std::shared_ptr<TcpClient> client);
    void AddTcpServer(std::int64_t index, std::shared_ptr<TcpServer> server);

    void SetEventFunc(DllEvent& dll_event);

private:
    std::atomic<std::int64_t>       m_next_index;
    std::mutex                      m_mtx;
    std::unordered_map<std::int64_t, std::shared_ptr<TcpClient>> m_client_map;
    std::unordered_map<std::int64_t, std::shared_ptr<TcpServer>> m_server_map;
};

