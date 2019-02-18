#pragma once

#include <ctime>
#include <list>
#include <mutex>
#include <deque>
#include <sys/time.h>

#include "Connection.h"

namespace rediscpp {

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

struct ConnectionOpt
{
    ConnectionOpt() = default;
    ~ConnectionOpt() = default;
    ConnectionOpt(const ConnectionOpt& rhs) = default;
    ConnectionOpt& operator=(const ConnectionOpt& rhs) = default;
    ConnectionOpt(ConnectionOpt&& rhs) = default;
    ConnectionOpt& operator=(ConnectionOpt&& rhs) = default;

    std::string m_ip{};
    int         m_port{};
    std::shared_ptr<timeval> m_time_out{};
};

struct ConnectionPoolOpt
{
	ConnectionPoolOpt() = default;
	~ConnectionPoolOpt() = default;
	ConnectionPoolOpt(const ConnectionPoolOpt& rhs) = default;
	ConnectionPoolOpt& operator=(const ConnectionPoolOpt& rhs) = default;
	ConnectionPoolOpt(ConnectionPoolOpt&& rhs) = default;
	ConnectionPoolOpt& operator=(ConnectionPoolOpt&& rhs) = default;

	size_t m_pool_size{3};           //线程池初始线程个数
	size_t m_pool_max_size{3};       //线程池最大线程个数
};

class ConnectionPool
{
    struct Slot
    {
        Slot(ConnectionPtr conn, bool is_using)
            : m_conn(std::move(conn))
            , m_is_using(is_using)
        {
        }

        ConnectionPtr m_conn{};
        bool          m_is_using{};
    };
    using SlotPtr = std::shared_ptr<Slot>;

public:
	ConnectionPool(ConnectionOpt conn_opt, ConnectionPoolOpt pool_opt = {});
    ~ConnectionPool();

    ConnectionPool(const ConnectionPool& rhs) = delete;
	ConnectionPool& operator=(const ConnectionPool& rhs) = delete;

	bool init();
    ConnectionPtr getConn();
    void rleaseConn(ConnectionPtr conn);
    size_t connectionCount() const;
private:
    ConnectionPtr create() const;
    ConnectionPool::SlotPtr findEmptySlot();

private:
    mutable std::mutex      m_mutex;
    std::list<SlotPtr>      m_pool;
    ConnectionOpt			m_conn_opt;
	ConnectionPoolOpt		m_pool_opt;
};

} // rediscpp
