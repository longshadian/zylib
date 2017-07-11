#ifndef _MYSQLCPP_CONNECTIONPOOL_H
#define _MYSQLCPP_CONNECTIONPOOL_H

#include <ctime>
#include <list>
#include <mutex>

#include "Types.h"
#include "Connection.h"

namespace mysqlcpp {

class Connection;
class ConnectionPool;

class ConnectionGuard
{
public:
    ConnectionGuard(ConnectionPool& pool);
    ~ConnectionGuard();

	Connection& operator*() const { return *m_conn; }
	const std::shared_ptr<Connection>& operator->() const { return m_conn; }
	operator bool() const { return m_conn != nullptr; }

	Connection* get() const { return m_conn.get(); }
private:
    ConnectionPool&  m_pool;
	std::shared_ptr<Connection> m_conn;
};

struct ConnectionPoolOpt
{
	ConnectionPoolOpt() = default;
	~ConnectionPoolOpt() = default;
	ConnectionPoolOpt(const ConnectionPoolOpt& rhs) = default;
	ConnectionPoolOpt& operator=(const ConnectionPoolOpt& rhs) = default;
	ConnectionPoolOpt(ConnectionPoolOpt&& rhs) = default;
	ConnectionPoolOpt& operator=(ConnectionPoolOpt&& rhs) = default;

	size_t m_thread_pool_size{3};           //线程池初始线程个数
	size_t m_thread_pool_max_size{3};       //线程池最大线程个数
	size_t m_thread_pool_idle_timeout{0};   //多余空闲线程等待多久销毁(秒)
    size_t m_mysql_ping_seconds{ 0 };       // 连接建立多少秒没被使用后，检查ping mysql
	//size_t m_thread_pool_stall_limit{0};    //无线程可用时等待多久创建线程
};

class ConnectionPool
{
    struct Slot
    {
        Slot(std::shared_ptr<Connection> conn , time_t t, bool use)
            : m_conn(conn)
            , m_last_used(t)
            , m_in_use(use)
        {
        }

		std::shared_ptr<Connection>		m_conn;
        time_t                          m_last_used;
        bool                            m_in_use;
    };
    using SlotPtr = std::shared_ptr<Slot>;

public:
	ConnectionPool(ConnectionOpt conn_opt, ConnectionPoolOpt pool_opt = {});
    ~ConnectionPool() = default;

    ConnectionPool(const ConnectionPool& rhs) = delete;
	ConnectionPool& operator=(const ConnectionPool& rhs) = delete;

	bool init();

    std::shared_ptr<Connection> getConn();
    void rleaseConn(std::shared_ptr<Connection> conn);
private:
    SlotPtr getConnSlot();
    void checkAutoReconn(SlotPtr& slot) const;

    std::shared_ptr<Connection> create() const;
    SlotPtr findEmptySlot();
	void destoryTimeout(time_t tnow);
private:
    std::mutex              m_mutex;
    std::list<SlotPtr>      m_pool;
    ConnectionOpt			m_conn_opt;
	ConnectionPoolOpt		m_pool_opt;
};

}

#endif
