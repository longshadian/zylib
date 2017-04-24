#ifndef _WORLD_WORLD_H_
#define _WORLD_WORLD_H_

#include <cstdint>
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

#include "WorldSession.h"
#include "WorldDefine.h"
#include "GameType.h"

#include "network/RWHandler.h"

class WorldMsgDispatcher;

class World
{
    enum { WORLD_WAIT_SLEEP_MILLISECONDS = 50 };
public:
                    World();
	                ~World();
    //������¼�
    void            networkAccept(WorldConnection hdl);
    void            networkReceviedMsg(WorldConnection hdl, std::shared_ptr<NetworkMsg>  msg);
    void            networkTimeout(WorldConnection hdl);
    void            networkClosed(WorldConnection hdl);
public:
    bool            init();
	void            heartbeat(uint32_t diff);
    void            stop();
    void            waitStop();
    bool            isRunning() const;
    void            shutdownSession(WorldSession* session);
public:
    void            postTask(std::function<void()> task);
private:
    void            run();
    void            dispatchMsg();
    void            processNewConn();
    void            processClosedConn();
    void            processTask();
    void            destroyession(WorldSessionPtr session);
    WorldSessionPtr findSession(const WorldConnection& conn);
private:
    mutable std::mutex            m_mtx;
    std::unique_ptr<WorldMsgDispatcher> m_msg_dispatcher;

    std::deque<std::shared_ptr<WorldMsg>>  m_world_msgs;        //������յ���Э���
    std::map<WorldConnection, WorldSessionPtr, WorldConnectionLess>    m_sessions;       //��������
    std::set<WorldSessionPtr>     m_new_conn;       //�µ�����,��δ���� 
    std::set<WorldConnection, WorldConnectionLess> m_closed_conn;    //�Ѿ��رյ�����,��û����

    std::atomic<bool>                             m_is_running;
    std::thread                                   m_thread;
    std::list<std::function<void()>>              m_task;    //��Ҫ��ѭ����������
};

#endif
