#include "DataBaseService.h"

#include "mysqlcpp/MySqlCpp.h"
#include "zylib/AsyncTask.h"

#include "Config.h"
#include "Log.h"

DataBaseService::DataBaseService()
	: m_mtx()
    , m_cond()
    , m_queue()
    , m_running(false)
    , m_threads()
{
}

DataBaseService::~DataBaseService()
{
    stop();
    waitExit();
}

bool DataBaseService::init(size_t num_threads)
{
    if (m_running.exchange(true))
        return false;
    const auto* db_conf = config::instance().getDataBaseConf();
    for (const auto& it : db_conf->m_pools) {
        config::DBPool& pool_conf = *it.second;

        mysqlcpp::ConnectionOpt conn_opt{};
        conn_opt.user = pool_conf.m_user;
        conn_opt.password = pool_conf.m_password;
        conn_opt.database = pool_conf.m_name;
        conn_opt.host = pool_conf.m_ip;
        conn_opt.port = pool_conf.m_port;
        conn_opt.auto_reconn = true;

        mysqlcpp::ConnectionPoolOpt conn_pool_opt{};
        conn_pool_opt.m_thread_pool_size = pool_conf.m_init_size;
        conn_pool_opt.m_thread_pool_max_size = pool_conf.m_max_size;

        auto pool = std::make_shared<mysqlcpp::ConnectionPool>(std::move(conn_opt), std::move(conn_pool_opt));
        if (!pool->init()) {
            LOG(ERROR) << "db pool init error:" << pool_conf.m_id << ':' << pool_conf.m_ip << ':';
            return false;
        }
        m_conn_pools[pool_conf.m_id] = pool;
    }

	for (size_t i = 0; i != num_threads; ++i) {
		m_threads.push_back(std::thread(&DataBaseService::threadStart, this));
	}
    return true;
}

void DataBaseService::stop()
{
    if (!m_running.exchange(false))
        return;
    std::lock_guard<std::mutex> lk(m_mtx);
	for (size_t i = 0; i != m_threads.size(); ++i) {
		m_queue.push(zylib::AsyncTask());
	}
    m_cond.notify_all();
}

void DataBaseService::waitExit()
{
	for (auto& thread : m_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

void DataBaseService::executeRemainTask()
{
	if (m_running)
		return;
    std::lock_guard<std::mutex> lk(m_mtx);
    while (!m_queue.empty()) {
        zylib::AsyncTask task;
        task = std::move(m_queue.front());
        m_queue.pop();
        if (task) {
            task();
        }
    }
}

void DataBaseService::threadStart()
{
    try {
        run();
    } catch (...) {
    }
}

void DataBaseService::run()
{
    while (m_running) {
        zylib::AsyncTask task{};
        {
            std::unique_lock<std::mutex> lk(m_mtx);
            m_cond.wait(lk, [this] { return !m_queue.empty(); });
            task = std::move(m_queue.front());
            m_queue.pop();
        }
        if (task) {
            task();
        }
    }
}

int DataBaseService::countDetail(int val)
{
    mysqlcpp::ConnectionGuard conn_guard{*m_conn_pools[0]};
    if (conn_guard) {
        Conn cn(*conn_guard);

        int n = 10;
        while (--n > 0) {
            std::cout << "n:" << n << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return cn.exe(val);
    }
    std::cout << "conn error\n";
    return 0;
}

std::future<int> DataBaseService::countID(int val)
{
	return asyncSubmit([this, val]() -> int
	{
        try {
            return countDetail(val);
        } catch (uint32_t mysql_errno) {
            std::cout << "mysql errno:" << mysql_errno << "\n";
        }
        return 1;
	});
}
