#include "DataBaseService.h"

#include "MySqlCpp.h"

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

	mysqlcpp::ConnectionOpt conn_opt{};
	conn_opt.user = "root";
	conn_opt.password = "123456";
	conn_opt.database = "test";
	conn_opt.host = "127.0.0.1";
	conn_opt.port = 3306;

	mysqlcpp::ConnectionPoolOpt conn_pool_opt{};

	m_db_pool = std::make_shared<mysqlcpp::ConnectionPool>(conn_opt, conn_pool_opt);
	if (!m_db_pool->init()) {
		std::cout << "error:db pool init\n";
		return false;
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
    mysqlcpp::ConnectionGuard conn_guard{*m_db_pool};
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
