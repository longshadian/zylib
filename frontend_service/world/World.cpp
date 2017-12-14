#include "World.h"

#include <memory>

#include "WorldMsg.h"
#include "WorldMsgDispatcher.h"

const std::chrono::milliseconds WORLD_HEARTBEAT_RATE{50};

World::World()
	: m_mtx()
	, m_world_msgs()
	, m_is_running()
	, m_thread()
{
}

World::~World()
{
    m_is_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void World::networkAccept(Hdl conn)
{
    (void)conn;
    //LOG(DEBUG) << "networkAccept: [" << hdl << "] ip:" << conn_info->m_ip;
}

void World::networkReceviedMsg(Hdl conn, std::shared_ptr<Message> network_msg)
{
    (void)conn;
    (void)network_msg;
}

void World::networkTimeout(Hdl conn)
{
    (void)conn;
}

void World::networkClosed(Hdl conn)
{
    (void)conn;
}

bool World::init()
{
    if (m_is_running)
        return false;

    // 1.初始化基础管理功能

    // 2.启动游戏服务
	// 镇江麻将

    m_thread = std::thread([this]
        {
            m_is_running = true;
            run();
        });
    return true;
}

void World::run()
{
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

void World::stop()
{
    m_is_running.store(false);
}

void World::waitTheadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

bool World::isRunning() const
{
    return m_is_running;
}

void World::shutdownSession(WorldSession* session)
{
    if (!session)
        return;
    auto conn = session->getConnection();
    /*
    if (!conn)
        return;
    */
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_closed_conn.insert(conn);
    }
}

void World::postTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_task.push_back(std::move(task));
}

TimerManager& World::getTimerManager()
{
	return *m_timer_manager;
}

zylib::DefaultRandom& World::getRandom()
{
    return *m_random;
}

void World::heartbeat(DiffTime diff)
{
}

void World::dispatchMsg()
{
    decltype(m_world_msgs) all_cbs{};
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        std::swap(all_cbs, m_world_msgs);
    }
}
