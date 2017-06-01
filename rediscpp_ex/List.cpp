#include "List.h"

#include "Exception.h"
#include "Utile.h"
#include "Connection.h"
#include "Buffer.h"

namespace rediscpp {

List::List(Connection& conn)
    : m_conn(conn)
{}

long long List::LLEN(Buffer key)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"LLEN %b", key.getData(), key.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("LLEN reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("LLEN type REDIS_REPLY_INTEGER");
    return r->integer;
}

Buffer List::LPOP(Buffer key)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"LPOP %b", key.getData(), key.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("LPOP reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("LPOP type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

Buffer List::RPOP(Buffer key)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"RPOP %b", key.getData(), key.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("RPOP reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("RPOP type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

std::vector<Buffer> List::LRANGE(Buffer key, int start, int stop)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(), "LRANGE %b %d %d", key.getData(), key.getLen(), start, stop))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("LRANGE reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("LRANGE type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(r, r->elements);
}

long long List::LPUSH(Buffer key, Buffer val)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(), "LPUSH %b %b", key.getData(), key.getLen(), val.getData(), val.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("LPUSH reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("LPUSH type REDIS_REPLY_INTEGER");
    return r->integer;
}

long long List::RPUSH(Buffer key, Buffer val)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(), "RPUSH %b %b", key.getData(), key.getLen(),
            val.getData(), val.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("RPUSH reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("RPUSH type REDIS_REPLY_INTEGER");
    return r->integer;
}

void List::LTRIM(Buffer key, int start, int stop)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(), "LTRIM %b %d %d", key.getData(), key.getLen(),
            start, stop))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("LTRIM reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("LTRIM type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("LTRIM type REDIS_REPLY_STATUS != OK");
}

Buffer List::LINDEX(Buffer key, int index)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(), "LINDEX %b %d", key.getData(), key.getLen(),
            index))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("LINDEX reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("LINDEX type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

}