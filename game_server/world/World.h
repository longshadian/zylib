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
#include "GameType.h"
#include "GameConnection.h"

class RWHandler;

class World
{
    enum { WORLD_WAIT_SLEEP_MILLISECONDS = 50 };
                            World();
public:
    static World&   instance();
	                ~World();
    //������¼�
    void            networkAccept(WorldConnection hdl, std::shared_ptr<WorldConnectionInfo> conn_info);
    void            networkReceviedMsg(WorldConnection hdl, WorldMessagePtr msg_cb);
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
    void            httpServerInfo(http::ResponsePtr response);
	void	        httpOnlineCount(http::ResponsePtr response);
    void            httpAccountLock(http::ResponsePtr response);
private:
    void            run();
    void            afterRun();
    void            dispatchGameMessage();
    void            processNewConn();
    void            processClosedConn();
    void            processTask();
    void            destroyession(WorldSessionPtr session);
    WorldSessionPtr findSession(const WorldConnection& conn);
    void            onlineInfo();
	GameModulePtr   getAllModuleInfo();
private:
    mutable std::mutex                            m_mtx;
    std::deque<GameMessageCBPtr>                  m_msg_cbs;        //������յ���Э���
    std::map<WorldConnection, WorldSessionPtr>    m_sessions;       //��������
    std::set<WorldSessionPtr>                     m_new_conn;       //�µ�����,��δ����
    std::set<WorldConnection>                     m_closed_conn;    //�Ѿ��رյ�����,��û����

    std::atomic<bool>                             m_is_running;
    std::thread                                   m_thread;
    std::list<std::function<void()>>              m_task;    //��Ҫ��ѭ����������
};

#endif
