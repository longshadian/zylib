#include "List.h"

#include "Exception.h"
#include "Utile.h"
#include "Connection.h"
#include "Buffer.h"

namespace rediscpp {

RedisList::RedisList(Connection& context)
    : m_context(context)
{}

long long RedisList::LLEN(Buffer key)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"LLEN %b", key.getData(), key.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("LLEN reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("LLEN type REDIS_REPLY_INTEGER");
    return r->integer;
}

Buffer RedisList::LPOP(Buffer key)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"LPOP %b", key.getData(), key.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("LPOP reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("LPOP type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

Buffer RedisList::RPOP(Buffer key)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"RPOP %b", key.getData(), key.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("RPOP reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("RPOP type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

std::vector<Buffer> RedisList::LRANGE(Buffer key, int start, int stop)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "LRANGE %b %d %d", key.getData(), key.getLen(), start, stop))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("LRANGE reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("LRANGE type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(r, r->elements);
}

long long RedisList::LPUSH(Buffer key, Buffer val)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "LPUSH %b %b", key.getData(), key.getLen(), val.getData(), val.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("LPUSH reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("LPUSH type REDIS_REPLY_INTEGER");
    return r->integer;
}

long long RedisList::RPUSH(Buffer key, Buffer val)
{
    Reply reply{reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "RPUSH %b %b", key.getData(), key.getLen(),
            val.getData(), val.getLen()))};
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("RPUSH reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("RPUSH type REDIS_REPLY_INTEGER");
    return r->integer;
}

}