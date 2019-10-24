#pragma once

#include <cstdint>
#include <atomic>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <network/Network.h>

#include "DllDefine.h"

class TcpClient;
class TcpServer;
class MessageDecoder;
class NetworkEvent;
class DllEvent;
class TcpClientWrapper;

class GlobalInstance
{
    GlobalInstance();
public:
    ~GlobalInstance();

    static GlobalInstance* Get();
    void Init();
    void Cleanup();

    ResultCode CreateClient(ActorID* out);
    ResultCode Connect(ActorID id, const char* host, uint16_t port);




    std::int64_t CreateClient(std::string host, std::uint16_t port);
    bool TcpLaunch(std::int64_t index);

    TcpConnectorWrapperPtr CreateConnector();

    ActorID NextIndex();

    std::shared_ptr<TcpClientWrapper> m_tcp_client_wrapper;

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
};

