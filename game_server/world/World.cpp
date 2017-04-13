#include "World.h"

#include <json/json.h>

#include "zylib/Random.h"
#include "GameTool.h"
#include "ServerApp.h"
#include "AppLog.h"
#include "GameMessage.h"
#include "GameMessageDispatcher.h"
#include "GameConnection.h"
#include "PerformanceMonitor.h"
#include "GameUser.h"
#include "GameAssert.h"
#include "ProtoMsg.h"
#include "RedisManager.h"

#include "GameModule.h"
#include "GlobalService.h"
#include "NetworkService.h"
#include "UserService.h"
#include "TimerService.h"
#include "ZjhRoomService.h"
#include "DBSaveStatis.h"
#include "DataBaseService.h"

#include "OnlineStatis.h"

#include "msg_hall.pb.h"

World::World()
    : m_mtx()
    , m_msg_cbs()
    , m_sessions()
    , m_new_conn()
    , m_closed_conn()
    , m_is_running(false)
    , m_thread()
    , m_task()
{
}

World& World::instance()
{
    static World instance_;
    return instance_;
}

World::~World()
{
    m_is_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void World::networkAccept(WorldConnection conn, std::shared_ptr<WorldConnectionInfo> conn_info)
{
    appLog(LOG_DEBUG, "%d %d", conn.m_fd, conn.m_seq_id);
    auto session = std::make_shared<WorldSession>(conn, conn_info);
    std::lock_guard<std::mutex> lk(m_mtx);
    m_new_conn.insert(session);
}

void World::networkReceviedMsg(WorldConnection conn, WorldMessagePtr world_msg)
{
    if (!world_msg)
        return;
    if (world_msg->m_msg_id == static_cast<int32_t>(share::MSG_ID::REQ_PING)) {
        pt::req_ping req{};
        if (!req.ParseFromArray(world_msg->m_buffer.data(), world_msg->m_buffer.size())) {
            appLog(LOG_ERROR, "parse error req_ping");
            return;
        }
        pt::rsp_ping rsp{};
        rsp.set_tm(req.tm());
        GlobalService::getNetworkService().sendMessage(conn, rsp);
        return;
    }

    auto session_fun = GameMessageDispatcher::getInstance().findRegCallback(world_msg->m_msg_id);
    if (!session_fun) {
        appLog(LOG_ERROR, "can't find reg callback %d %s", world_msg->m_msg_id, share::msgFullName(world_msg->m_msg_id));
        return;
    }

    auto game_msg_cb = std::make_shared<GameMessageCB>();
    game_msg_cb->m_session_fun = session_fun;
    game_msg_cb->m_msg = std::make_shared<GameMessage>();
    game_msg_cb->m_msg->m_timestamp = std::chrono::system_clock::now();
    game_msg_cb->m_msg->m_msg_id = world_msg->m_msg_id;
    game_msg_cb->m_msg->m_data = std::move(world_msg->m_buffer);
    std::lock_guard<std::mutex> lk(m_mtx);
    game_msg_cb->m_conn = conn;
    m_msg_cbs.push_back(std::move(game_msg_cb));
}

void World::networkTimeout(WorldConnection conn)
{
    appLog(LOG_DEBUG, "%d %d", conn.m_fd, conn.m_seq_id);
    std::lock_guard<std::mutex> lk(m_mtx);
    m_closed_conn.insert(conn);
}

void World::networkClosed(WorldConnection conn)
{
    appLog(LOG_DEBUG, "%d %d", conn.m_fd, conn.m_seq_id);
    std::lock_guard<std::mutex> lk(m_mtx);
    m_closed_conn.insert(conn);
}

bool World::init()
{
    if (m_is_running)
        return false;
    if (!GameMessageDispatcher::getInstance().init()) {
        return false;
    }
    m_thread = std::thread([this]
        {
            m_is_running = true;
            run();
            afterRun();
        });

    GlobalService::getTimerService().addTimer(std::bind(&World::onlineInfo, this), {1000* 60});
    GlobalService::getTimerService().addTimer(std::bind(&RedisManager::keepAlive, &g_redis_mgr), { RedisManager::KEEP_ALIVE_TIMER});
    GlobalService::getTimerService().addTimer(std::bind(&DataBaseService::keepAlive, &GlobalService::getDataBaseService()), { DataBaseService::KEEP_ALIVE_TIMER});
    return true;
}

void World::run()
{
    uint32_t real_current_time = 0;
    uint32_t real_previous_time = zylib::getMSTime();
    uint32_t previous_sleep_time = 0;

    //TODO 监控主循环压力
    const uint32_t timer_tick = 1000 * 60;
    zylib::MSTimerTracker timer(timer_tick);
    PerformanceMonitor monitor{};

    while (m_is_running) {
        real_current_time = zylib::getMSTime();
        auto diff_time = zylib::getMSTimeDiff(real_previous_time, real_current_time);
        heartbeat(diff_time);

        real_previous_time = real_current_time;
        if (diff_time < WORLD_WAIT_SLEEP_MILLISECONDS + previous_sleep_time) {
            previous_sleep_time = WORLD_WAIT_SLEEP_MILLISECONDS + previous_sleep_time - diff_time;
            std::this_thread::sleep_for(std::chrono::milliseconds(previous_sleep_time));

            monitor.tick(0);
        } else {
            previous_sleep_time = 0;
            auto delta = diff_time - (WORLD_WAIT_SLEEP_MILLISECONDS + previous_sleep_time);
            monitor.tick(delta);
        }

        timer.update(diff_time);
        if (timer.passed()) {
            timer.reset(timer_tick);

            /*
            appLog(LOG_DEBUG, "performance seconds:%d target_countor:%d "
                "countor:%d delta_countor:%d delta:%d delta_s:%d delta_max:%d",
                timer_tick/1000, timer_tick/WORLD_WAIT_SLEEP_MILLISECONDS,
                monitor.m_countor, monitor.m_delta_countor, monitor.m_delta, monitor.m_delta/1000,
                monitor.m_delta_max);
                */
            monitor.reset();
        }
    }
}

void World::stop()
{
    m_is_running.store(false);
}

void World::waitStop()
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
    if (!conn)
        return;
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_closed_conn.insert(conn);
    }
    GlobalService::getNetworkService().shutdownConn(conn);
}

void World::postTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_task.push_back(std::move(task));
}

void World::heartbeat(uint32_t diff)
{
    //1.处理新的链接
    processNewConn();

    //2.处理断开的链接
    processClosedConn();

    processTask();

    //3.分发请求
    dispatchGameMessage();

    //4.客户端逻辑
    for (auto it = m_sessions.begin(); it != m_sessions.end();) {
        auto session = it->second;
        if (!session->isClosed()) {
            session->heartbeat(diff);
        }
        if (session->isClosed()) {
            destroyession(session);
            m_sessions.erase(it++);
        } else {
            ++it;
        }
    }

    GlobalService::heartbeat(diff);
}

void World::afterRun()
{
}

void World::dispatchGameMessage()
{
    decltype(m_msg_cbs) all_cbs{};
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        std::swap(all_cbs, m_msg_cbs);
    }

    while (!all_cbs.empty()) {
        auto game_msg = std::move(all_cbs.front());
        all_cbs.pop_front();

        auto session = findSession(game_msg->m_conn);
        if (session) {
            FilterGameMessage filter{};
            if (filter(*session, *game_msg)) {
                appLog(LOG_ERROR, "%d filter error %s", game_msg->m_msg->m_msg_id, share::msgFullName(game_msg->m_msg->m_msg_id));
                continue;
            }
            session->postGameMessageCB(std::move(game_msg));
        } else {
            //plog(LOG_ERROR, "%d", game_msg->connID());
        }
    }
}

void World::processNewConn()
{
    decltype(m_new_conn) all_new_conn{};
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        std::swap(all_new_conn, m_new_conn);
    }
    for (auto it = all_new_conn.begin(); it != all_new_conn.end(); ++it) {
        m_sessions.insert({(*it)->getConnection(), *it});
    }
}

void World::processClosedConn()
{
    decltype(m_closed_conn) all_closed_conn{};
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        std::swap(all_closed_conn, m_closed_conn);
    }

    for (auto it = all_closed_conn.begin(); it != all_closed_conn.end(); ++it) {
        auto conn = *it;
        auto session_it = m_sessions.find(conn);
        if (session_it != m_sessions.end()) {
            auto& session = session_it->second;
            if (!session->isClosed()) {
                session->closed();
            }
        }
    }
}

void World::processTask()
{
    decltype(m_task) all_task{};
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        std::swap(all_task, m_task);
    }

    while (!all_task.empty()) {
        all_task.front()();
        all_task.pop_front();
    }
}

void World::destroyession(WorldSessionPtr session)
{
    session->userDisconnection();
    auto user = session->getGameUser();
    if (user) {
        auto userid = user->getUserid();
        //从online队列中删除，加入Agent队列
        GlobalService::getUserService().removeOnline(userid);
        GlobalService::getUserService().addAgent(user);
    }
}

WorldSessionPtr World::findSession(const WorldConnection& conn)
{
    auto it = m_sessions.find(conn);
    if (it != m_sessions.end())
        return it->second;
    return nullptr;
}

void World::onlineInfo()
{
	auto all_module_info = getAllModuleInfo();
    auto statis = gametool::makeUnique<OnlineStatis>();
    statis->m_online_log.create_time = std::time(nullptr);
    statis->m_online_log.hall = all_module_info->m_hall;
    statis->m_online_log.zjh  = all_module_info->getZjhTotalUser();

	appLog(LOG_DEBUG, "online hall:%d zjh:%d", statis->m_online_log.hall, statis->m_online_log.zjh);

    DBSaveStatis::instance().post(std::move(statis));
    GlobalService::getTimerService().addTimer(std::bind(&World::onlineInfo, this), {1000 * 60});

    /*
    for (int i = 0; i != 10; ++i) {
        auto val = zylib::rand<uint32_t>(1000, 10000);
        GlobalService::getTimerService().addTimer([val]() { appLog(LOG_DEBUG, "xxxxxx test %d", val); }, {val});
    }
    */
}

GameModulePtr World::getAllModuleInfo()
{
	auto game_module_info = gametool::makeUnique<GameModule>();
	game_module_info->m_zjh_room = GlobalService::getRoomService().allRoomInfo();

	//大厅人数
	for (const auto& s : m_sessions) {
		auto user = s.second->getGameUser();
		if (user && user->getGameid() == 0 && user->getTableid() == 0)
			game_module_info->m_hall++;
	}
	return game_module_info;
}