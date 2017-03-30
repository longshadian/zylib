#pragma once

#include <atomic>
#include <thread>
#include <memory>
#include <mutex>
#include <queue>
#include <future>
#include <set>
#include "AsyncTask.h"

#include "MySqlCpp.h"

#include <iostream>

struct Conn
{
	Conn(mysqlcpp::Connection& conn) : m_conn(conn) { }

	int exe(int val)
	{
		const char* sql = "SELECT count(fid) as cnt FROM mytest.ttest";
		auto ret = m_conn.query(sql);
		if (!ret) {
			std::cout << "ret error\n";
			return 0;
		}
		std::ostringstream ostm{};
		ostm << std::this_thread::get_id() << " success\n";
		std::cout << ostm.str();
		auto row = ret->getRow(0);
		return row["cnt"]->getInt32() + val;
	}

	mysqlcpp::Connection& m_conn;
};

class DataBaseService
{
public:
    DataBaseService();
    ~DataBaseService();

    DataBaseService(const DataBaseService& rhs) = delete;
    DataBaseService& operator=(const DataBaseService& rhs) = delete;

    bool init(size_t num_threads);
    void stop();
    void waitStop();
	void executeRemainTask();

	std::future<int> countID(int val);


    template<typename F>
    std::future<typename std::result_of<F()>::type> asyncSubmit(F f)
    {
        typedef typename std::result_of<F()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        auto res = task.get_future();
        std::lock_guard<std::mutex> lk(m_mtx);
        m_queue.push(std::move(task));
        m_cond.notify_all();
        return res;
    }
private:
    void threadStart();
    void run();
private:
    std::mutex					m_mtx;
    std::condition_variable     m_cond;
    std::queue<zylib::AsyncTask> m_queue;
    std::atomic<bool>           m_running;
	std::vector<std::thread>    m_threads;
	std::shared_ptr<mysqlcpp::ConnectionPool> m_db_pool;
};
