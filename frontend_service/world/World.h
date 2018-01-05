#pragma once

#include <cstdint>
#include <ctime>
#include <thread>
#include <atomic>
#include <functional>

#include "Queue.h"
#include "net/NetworkType.h"
#include "world/WorldDefine.h"
#include "world/WorldDefine.h"
#include "world/Session.h"
#include "world/MsgDispatcher.h"

class World
{
public:
	using Task = std::function<void()>;
public:
	World();
	~World();
	World(const World& rhs) = delete;
	World& operator=(const World& rhs) = delete;
	World(World&& rhs) = delete;
	World& operator=(World&& rhs) = delete;

    // ÍøÂç²ãÊÂ¼þ
    void                    NetworkAccept(Hdl hdl);
    void                    NetworkReceviedMsg(Hdl hdl, std::shared_ptr<CSMessage> msg);
    void                    NetworkTimeout(Hdl hdl);
    void                    NetworkClosed(Hdl hdl);

    void                    PostTask(Task t);
public:
    bool                    Init();
    void                    Stop();
    void                    WaitTheadExit();
    bool                    IsRunning() const;
    void                    SetClientCallback(ClientCB_Array arr);

    void                    shutdownClientSession(Hdl hdl);
    void                    SendMessage(uint64_t uid);

private:
    void                    Run();
    void                    Tick(DiffTime diff);
    void                    OnReceivedClientMsg(Hdl hdl, std::shared_ptr<CSMessage> msg);
    void                    OnClientAccept(Hdl hdl);
    void                    OnClientClosed(Hdl hdl);
    void                    OnClientTimeout(Hdl hdl);
    void                    ProcessTask();

    void                    CallClientMessage(Hdl hdl, std::shared_ptr<CSMessage> msg) const;

    ClientSessionPtr        FindSession(const Hdl& hdl);

private:
    std::atomic<bool>       m_is_running;
    std::thread             m_thread;
    std::unordered_map<Hdl, ClientSessionPtr, HdlLess>  m_client_sessions;
    std::unordered_map<ConnID, ClientSessionPtr>        m_client_conns;
    ClientCB_Array          m_client_cb_array;
    ConnID                  m_conn_id;

    std::mutex              m_mtx;
    std::queue<Task>        m_queue;
};
