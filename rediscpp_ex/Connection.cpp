#include "Connection.h"

#include <cstring>

#include "Utile.h"
#include "Exception.h"

namespace rediscpp {

Reply::Reply() : Reply(nullptr) {}

Reply::Reply(redisReply* redis_reply) 
    : m_redis_reply(redis_reply) 
{
}

Reply::~Reply()
{
    if (m_redis_reply)
        ::freeReplyObject(m_redis_reply);
}

Reply::Reply(Reply&& rhs)
    : m_redis_reply(rhs.m_redis_reply)
{
    rhs.m_redis_reply = nullptr;
}

Reply& Reply::operator=(Reply&& rhs)
{
    if (this != &rhs) {
        std::swap(m_redis_reply, rhs.m_redis_reply);
    }
    return *this;
}

Reply::operator bool() const
{
    return m_redis_reply != nullptr;
}

redisReply* Reply::getRedisReply() { return m_redis_reply; }


//////////////////////////////////////////////////////////////////////////

Connection::Connection() : Connection(nullptr) {}

Connection::Connection(redisContext* redis_context)
    : m_redis_context(redis_context)
{
}

Connection::~Connection()
{
    shutdown();
}

Connection::Connection(Connection&& rhs)
    : m_redis_context(rhs.m_redis_context)
{
    rhs.m_redis_context = nullptr;
}

Connection& Connection::operator=(Connection&& rhs)
{
    if (this != &rhs) {
        std::swap(m_redis_context, rhs.m_redis_context);
    }
    return *this;
}

Connection::operator bool() const
{
    return m_redis_context != nullptr;
}

redisContext* Connection::getRedisContext()
{
    return m_redis_context;
}

bool Connection::reconnection()
{
    if (::redisReconnect(m_redis_context) != REDIS_OK)
        return false;
    return keepAlive();
}

void Connection::shutdown()
{
    if (m_redis_context)
        ::redisFree(m_redis_context);
    m_redis_context = nullptr;
}

bool Connection::keepAlive()
{
    try {
        auto str = PING(*this);
        if (str.empty())
            return false;
        return std::strcmp(str.c_str(), "PONG") == 0;
    } catch (const Exception& e) {
        (void)e;
        return false;
    }
}

}
