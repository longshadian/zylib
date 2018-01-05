#include "World.h"

#include <memory>

#include "Service.h"
#include "SIDManager.h"
#include "FakeLog.h"
#include "Utility.h"
#include "world/WorldMsg.h"

#include <knet/knet.h>

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
    PostTask(std::bind(&World::OnClientAccept, this, std::move(conn)));
}

void World::NetworkReceviedMsg(Hdl conn, std::shared_ptr<CSMessage> msg)
{
    // 发送给自己
    auto sid = msg->GetSID();
    if (sid.empty() || sid == ::GetKNet().GetServiceID()) {
        PostTask(std::bind(&World::OnReceivedClientMsg, this, std::move(conn), std::move(msg)));
        return;
    }

    // 判断sid 是否合法
    if (!::GetSIDManager().IsValidSID(sid)) {
        LOG(WARNING) << "unknown sid: " << sid;
        return;
    }

    // 转发
    ::GetKNet().RouteMessage(std::move(sid), msg->GetMsgID(), msg->m_body);
}

void World::NetworkTimeout(Hdl conn)
{
    PostTask(std::bind(&World::OnClientTimeout, this, std::move(conn)));
}

void World::NetworkClosed(Hdl conn)
{
    PostTask(std::bind(&World::OnClientClosed, this, std::move(conn)));
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

    m_thread = std::thread([this]
        {
            m_is_running = true;
            Run();
        });
    return true;
}

void World::Run()
{
	zylib::TimePoint tp_current{};
	zylib::TimePoint tp_previous = zylib::getSteadyTimePoint();
	zylib::Delta previous_sleep_time{};
    const std::chrono::milliseconds WORLD_TICK_HZ = 10_MS;

    // 监控主循环压力
    while (m_is_running) {
		tp_current = zylib::getSteadyTimePoint();
		auto diff_time = zylib::getDelta(tp_previous, tp_current);
        Tick(diff_time);

        tp_previous = tp_current;
		if (diff_time < WORLD_TICK_HZ + previous_sleep_time) {
			previous_sleep_time = WORLD_TICK_HZ + previous_sleep_time - diff_time;
            std::this_thread::sleep_for(previous_sleep_time);
        } else {
			previous_sleep_time = {};
            auto delta = diff_time - (WORLD_TICK_HZ + previous_sleep_time);
            (void)delta;
        }
    }
}

void World::Tick(DiffTime diff)
{
    (void)diff;
    ProcessTask();
}

void World::OnReceivedClientMsg(Hdl hdl, std::shared_ptr<CSMessage> msg)
{
    auto ret = Utility::DecryptMessage(msg);
    if (!ret) {
        shutdownClientSession(std::move(hdl));
        FAKE_LOG(WARNING) << "decrpyt fail.";
        return;
    }
    CallClientMessage(std::move(hdl), std::move(msg));
}

void World::OnClientAccept(Hdl hdl)
{
    auto conn_id = ++m_conn_id;
    auto client_session = std::make_shared<ClientSession>(hdl, conn_id);
    m_client_sessions.insert({hdl, std::move(client_session)});
    m_client_conns.insert({conn_id, client_session});
}

void World::OnClientClosed(Hdl hdl)
{
    (void)hdl;
    // TODO 广播消息
}

void World::OnClientTimeout(Hdl hdl)
{
    (void)hdl;
    // TODO 广播消息
}

void World::ProcessTask()
{
    decltype(m_queue) queue_cp{};
    {
        std::lock_guard<std::mutex> lk{m_mtx};
        queue_cp = std::move(m_queue);
    }
    while (!queue_cp.empty()) {
        Task t = std::move(m_queue.front());
        m_queue.pop();
        t();
    }
}

void World::CallClientMessage(Hdl hdl, std::shared_ptr<CSMessage> msg) const
{
    auto msg_id = msg->GetMsgID();
    auto it = m_client_cb_array.find(msg_id);
    if (it == m_client_cb_array.end()) {
        FAKE_LOG(WARNING) << "can't find msg_id: " << msg_id;
        return;
    }
    it->second(std::move(hdl), std::move(msg));
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
        m_client_conns.erase(cs->GetUserID());
    m_client_sessions.erase(hdl);
}

void World::SendMessage(uint64_t uid)
{
    auto it = m_client_conns.find(uid);
    if (it == m_client_conns.end())
        return;
    it->second->SendMessage();
}

ClientSessionPtr World::FindSession(const Hdl& hdl)
{
    auto it = m_sessions.find(hdl);
    if (it == m_sessions.end())
        return nullptr;
    return it->second;
}
