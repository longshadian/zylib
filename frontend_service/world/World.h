#pragma once

#include <cstdint>
#include <ctime>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <queue>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <list>

#include "WorldDefine.h"
#include "net/NetworkType.h"

class World
{
public:
	using Task = std::function<void()>;
public:
	World();
	~World();
	World(const World& rhs) = delete;
	World& operator=(const World& rhs) = delete;

    //网络层事件
    void            networkAccept(Hdl hdl);
    void            networkReceviedMsg(Hdl hdl, std::shared_ptr<Message> msg);
    void            networkTimeout(Hdl hdl);
    void            networkClosed(Hdl hdl);

public:
    bool            init();
	void            heartbeat(DiffTime diff);
    void            stop();
    void            waitTheadExit();
    bool            isRunning() const;
    void            shutdownSession(WorldSession* session);

private:
    void            run();
    void            dispatchMsg();

private:
    mutable std::mutex                          m_mtx;
    std::deque<std::shared_ptr<WorldMsg>>		m_world_msgs;     //网络库收到的协议包
    std::atomic<bool>	                        m_is_running;
    std::thread                                 m_thread;
};
