#include "Connection.h"

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
    if (m_redis_context)
        ::redisFree(m_redis_context);
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

}
