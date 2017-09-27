#include "ConnectionPool.h"

#include <algorithm>
#include <cassert>

namespace rediscpp {

ConnectionGuard::ConnectionGuard(ConnectionPool& pool)
    : m_pool(pool)
    , m_conn(nullptr)
{
	m_conn = m_pool.getConn();
}

ConnectionGuard::~ConnectionGuard()
{
	if (m_conn) {
		m_pool.rleaseConn(std::move(m_conn));
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ConnectionPool::ConnectionPool(ConnectionOpt conn_opt, ConnectionPoolOpt pool_opt)
	: m_mutex()
	, m_pool()
	, m_conn_opt(std::move(conn_opt))
	, m_pool_opt(std::move(pool_opt))
{

}

ConnectionPool::~ConnectionPool()
{
}

bool ConnectionPool::init()
{
	for (size_t i = 0; i != m_pool_opt.m_pool_size; ++i) {
        auto conn = create();
		if (!conn) {
			return false;
		}
        auto slot = std::make_shared<Slot>(conn, false);
        std::lock_guard<std::mutex> lk{ m_mutex };
		m_pool.push_back(std::move(slot));
	}
	return true;
}

ConnectionPtr ConnectionPool::getConn()
{
    {
        std::lock_guard<std::mutex> lk{ m_mutex };
        auto slot = findEmptySlot();
        if (slot) {
            slot->m_is_using = true;
            return slot->m_conn;
        }

        if (m_pool.size() >= m_pool_opt.m_pool_max_size) {
            return nullptr;
        }
    }

    // 创建新链接
    auto conn = create();
    if (!conn)
        return nullptr;
    {
        std::lock_guard<std::mutex> lk{ m_mutex };
        m_pool.push_back(std::make_shared<Slot>(conn, true));
    }
    return conn;
}

void ConnectionPool::rleaseConn(ConnectionPtr conn)
{
    std::lock_guard<std::mutex> m_lk{ m_mutex };
    auto it = std::find_if(m_pool.begin(), m_pool.end(), [&conn](const SlotPtr& p) { return p->m_conn == conn; });
    assert(it != m_pool.end());
    auto slot = *it;
    assert(slot->m_is_using);
    slot->m_is_using = false;

    // 放到列表的最后
    m_pool.erase(it);
    m_pool.push_back(slot);

    // TODO 销毁超时链接
}

size_t ConnectionPool::connectionCount() const
{
    std::lock_guard<std::mutex> lk{ m_mutex };
    return m_pool.size();
}

ConnectionPtr ConnectionPool::create() const
{
    std::shared_ptr<Connection> conn = nullptr;
    if (m_conn_opt.m_time_out) {
        conn = std::make_shared<Connection>(::redisConnectWithTimeout(m_conn_opt.m_ip.c_str(), m_conn_opt.m_port, *m_conn_opt.m_time_out));
    } else {
        conn = std::make_shared<Connection>(::redisConnect(m_conn_opt.m_ip.c_str(), m_conn_opt.m_port));
    }
    if (!*conn) {
        return nullptr;
    }
    if (!conn->keepAlive()) {
        return nullptr;
    }
    return conn;
}

ConnectionPool::SlotPtr ConnectionPool::findEmptySlot()
{
    for (auto& s : m_pool) {
        if (!s->m_is_using) {
            return s;
        }
    }
    return nullptr;
}

} // rediscpp

