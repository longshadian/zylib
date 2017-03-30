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

	size_t m_thread_pool_size{3};          //�̳߳س�ʼ�̸߳���
	size_t m_thread_pool_max_threads{5};   //�̳߳�����̸߳���
	size_t m_thread_pool_stall_limit{0};   //���߳̿���ʱ�ȴ���ô����߳�
	size_t m_thread_pool_idle_timeout{0};  //��������̵߳ȴ��������(����)
};

class ConnectionPool
{
    struct Slot
    {
		std::shared_ptr<Connection>		m_conn;
        time_t                          m_last_used;
        bool                            m_in_use;
        bool operator<(const Slot& rhs) const
        {
            const Slot& lhs = *this;
            return lhs.m_in_use == rhs.m_in_use ?  lhs.m_last_used < rhs.m_last_used : lhs.m_in_use; 
        }
    };
public:
	ConnectionPool(ConnectionOpt conn_opt, ConnectionPoolOpt pool_opt = {});
    ~ConnectionPool() = default;

    ConnectionPool(const ConnectionPool& rhs) = delete;
	ConnectionPool& operator=(const ConnectionPool& rhs) = delete;

	bool init();

    std::shared_ptr<Connection> getConn();
    void rleaseConn(std::shared_ptr<Connection> conn);
private:
    std::shared_ptr<Connection> create();
    Slot* findSlot(const std::shared_ptr<Connection>& conn);
    Slot* findEmptySlot();
	void destoryTimeout(time_t tnow);
private:
    std::mutex              m_mutex;
    std::list<Slot>         m_pool;
    ConnectionOpt			m_conn_opt;
	ConnectionPoolOpt		m_pool_opt;
};

}

#endif
