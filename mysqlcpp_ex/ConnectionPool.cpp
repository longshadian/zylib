#include "ConnectionPool.h"

#include <algorithm>

#include "MyAssert.h"
#include "FakeLog.h"
#include "Statement.h"

namespace mysqlcpp {

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

ConnectionPool::ConnectionPool(ConnectionOpt conn_opt, ConnectionPoolOpt pool_opt)
	: m_mutex()
	, m_pool()
	, m_conn_opt(std::move(conn_opt))
	, m_pool_opt(std::move(pool_opt))
{

}

bool ConnectionPool::init()
{
	for (size_t i = 0; i != m_pool_opt.m_thread_pool_size; ++i) {
		auto conn = std::make_shared<Connection>(m_conn_opt);
		if (conn->open() != 0) {
			FAKE_LOG(ERROR) << "create mysql conn error";
			return false;
		}

        auto slot = std::make_shared<Slot>(conn, std::time(nullptr), false);
		m_pool.push_back(slot);
	}
	return true;
}

std::shared_ptr<Connection> ConnectionPool::getConn()
{
    auto slot = getConnSlot();
    if (!slot)
        return nullptr;

    // 检查是否需要重连
    if (m_pool_opt.m_mysql_ping_seconds != 0)
        checkAutoReconn(slot);
    return slot->m_conn;
}

std::shared_ptr<Connection> ConnectionPool::create() const
{
    auto conn = std::make_shared<Connection>(m_conn_opt);
    if (conn->open() == 0)
        return conn;
    return nullptr;
}

void ConnectionPool::rleaseConn(std::shared_ptr<Connection> conn)
{
	auto tnow = std::time(nullptr);
	std::lock_guard<std::mutex> m_lk{ m_mutex };

    auto it = std::find_if(m_pool.begin(), m_pool.end(), [&conn](const SlotPtr& p) { return p->m_conn == conn; });
    MY_ASSERT(it != m_pool.end());
    auto slot = *it;
    MY_ASSERT(slot->m_in_use);
    slot->m_in_use = false;
	slot->m_last_used = tnow;
    m_pool.erase(it);
    m_pool.push_back(slot);

	// 销毁超时链接
    /*
    if (m_pool.size() > m_pool_opt.m_thread_pool_max_size) {
        destoryTimeout(tnow);
    }
    */
}

ConnectionPool::SlotPtr ConnectionPool::getConnSlot()
{
    {
		std::lock_guard<std::mutex> lk{ m_mutex };
        auto slot = findEmptySlot();
		if (slot) {
			slot->m_in_use = true;
            return slot;
		}

		if (m_pool.size() >= m_pool_opt.m_thread_pool_max_size) {
			FAKE_LOG(WARNING) << "too much connection! count:" << m_pool.size();
			return nullptr;
		}
    }

	//创建新数据库链接
	auto conn = create();
	if (!conn) {
		FAKE_LOG(ERROR) << "can't create new mysql connection error!";
		return nullptr;
	}

    auto new_slot = std::make_shared<Slot>(conn, std::time(nullptr), true);
	{
		//TODO 再次检测链接数量
		std::lock_guard<std::mutex> lk{ m_mutex };
		m_pool.push_back(new_slot);
	}
    return new_slot;
}

void ConnectionPool::checkAutoReconn(SlotPtr& slot) const
{
    auto tnow = std::time(nullptr);
    if (tnow - slot->m_last_used < static_cast<time_t>(m_pool_opt.m_mysql_ping_seconds)) {
        return;
    }
    std::shared_ptr<Connection> conn = slot->m_conn;
    auto stmt = conn->createStatement();
    auto rs = stmt->executeQuery("SELECT 1");
    if (rs) {
        FAKE_LOG(INFO) << "check conn success " << m_conn_opt.host;
        return;
    }
    auto new_conn = create();
    if (new_conn) {
        FAKE_LOG(INFO) << "auto reconn create success " << m_conn_opt.host;
        slot->m_conn = new_conn;
    } else {
        FAKE_LOG(ERROR) << "auto reconn create fail " << m_conn_opt.host;
    }
}

ConnectionPool::SlotPtr ConnectionPool::findEmptySlot()
{
    for (auto& s : m_pool) {
        if (!s->m_in_use)
            return s;
    }
    return nullptr;
}

void ConnectionPool::destoryTimeout(time_t tnow)
{
	for (auto it = m_pool.begin(); it != m_pool.end();) {
		const SlotPtr& slot = *it;
		if (slot->m_in_use) {
			++it;
			continue;
		}
		if (tnow - slot->m_last_used >= static_cast<time_t>(m_pool_opt.m_thread_pool_idle_timeout)) {
			it = m_pool.erase(it);
		} else {
			++it;
		}
	}
}

}

