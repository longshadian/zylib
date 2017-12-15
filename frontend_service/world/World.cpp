#include "World.h"

#include <memory>

#include "FakeLog.h"
#include "Utility.h"
#include "world/WorldMsg.h"

World::World()
    : m_is_running()
	, m_thread()
    , m_queue()
{
}

World::~World()
{
    m_is_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void World::NetworkAccept(Hdl conn)
{
    (void)conn;
    //LOG(DEBUG) << "networkAccept: [" << hdl << "] ip:" << conn_info->m_ip;
}

void World::NetworkReceviedMsg(Hdl conn, std::shared_ptr<Message> network_msg)
{
    (void)conn;
    (void)network_msg;
}

void World::NetworkTimeout(Hdl conn)
{
    (void)conn;
}

void World::NetworkClosed(Hdl conn)
{
    (void)conn;
}

void World::PostTask(Task t)
{
    m_queue.push(std::move(t));
}

bool World::Init()
{
    if (m_is_running)
        return false;

    // 1.初始化基础管理功能

    // 2.启动游戏服务
	// 镇江麻将

    m_thread = std::thread([this]
        {
            m_is_running = true;
            Run();
        });
    return true;
}

void World::Run()
{
    Task t{};
    while (m_is_running) {
        t = {};
        m_queue.waitAndPop(t, std::chrono::seconds{1});
        try {
            if (t)
                t();
        } catch (std::exception& e) {
            FAKE_LOG(WARNING) << "exception: " << e.what();
        }
    }

    /*
	zylib::TimePoint tp_current{};
	zylib::TimePoint tp_previous = zylib::getSteadyTimePoint();
	zylib::Delta previous_sleep_time{};

    // 监控主循环压力
	zylib::TimingWheel tw(30_s);

    while (m_is_running) {
		tp_current = zylib::getSteadyTimePoint();
		auto diff_time = zylib::getDelta(tp_previous, tp_current);
		heartbeat(diff_time);

        tp_previous = tp_current;
		if (diff_time < WORLD_HEARTBEAT_RATE + previous_sleep_time) {
			previous_sleep_time = WORLD_HEARTBEAT_RATE + previous_sleep_time - diff_time;
            std::this_thread::sleep_for(previous_sleep_time);
        } else {
			previous_sleep_time = {};
            auto delta = diff_time - (WORLD_HEARTBEAT_RATE + previous_sleep_time);
            (void)delta;
        }

        tw.update(diff_time);
        if (tw.passed()) {
			tw.reset();
			//LOG(DEBUG) << "performance countor";
        }
    }
    */
}

void World::processClientMessage(Hdl hdl, std::shared_ptr<Message> msg)
{
    auto ret = Utility::DecryptMessage(msg);
    if (!ret) {
        shutdownClientSession(std::move(hdl));
        FAKE_LOG(WARNING) << "decrpyt fail.";
        return;
    }
    auto sid = msg->GetSID();
    if (sid.empty()) {
        CallClientMessage(std::move(hdl), std::move(msg));
    } else {
        RouterClientMessage(std::move(hdl), std::move(msg));
    }
}

void World::CallClientMessage(Hdl hdl, std::shared_ptr<Message> msg) const
{
    auto msg_id = msg->GetMsgID();
    auto it = m_client_cb_array.find(msg_id);
    if (it == m_client_cb_array.end()) {
        FAKE_LOG(WARNING) << "can't find msg_id: " << msg_id;
        return;
    }
    it->second(std::move(hdl), std::move(msg));
}

void World::RouterClientMessage(Hdl hdl, std::shared_ptr<Message> msg)
{
    (void)hdl;
    (void)msg;
}

void World::Stop()
{
    m_is_running.store(false);
}

void World::WaitTheadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

bool World::IsRunning() const
{
    return m_is_running;
}

void World::SetClientCallback(ClientCB_Array arr)
{
    m_client_cb_array = std::move(arr);
}

void World::shutdownClientSession(Hdl hdl)
{
    auto handler = hdl.lock();
    if (handler)
        handler->shutdown();
    auto it = m_client_sessions.find(hdl);
    if (it == m_client_sessions.end())
        return;
    ClientSessionPtr cs = it->second;
    if (cs->GetUserID() != 0)
        m_clinet_users.erase(cs->GetUserID());
    m_client_sessions.erase(hdl);
}

void World::SendMessage(uint64_t uid)
{
    auto it = m_clinet_users.find(uid);
    if (it == m_clinet_users.end())
        return;
    it->second->SendMessage();
}
