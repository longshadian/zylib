#include "Zset.h"

#include "Exception.h"
#include "Utile.h"
#include "Connection.h"
#include "Buffer.h"

namespace rediscpp {

Zset::Zset(Connection& context)
    : m_context(context)
{}

long long Zset::ZADD(Buffer key, long long score, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"ZADD %b %lld %b", key.getData(), key.getLen(),
            score, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("ZADD reply null");
    if (r->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("ZADD REDIS_REPLY_INTEGER");
    return r->integer;
}

std::vector<Buffer> Zset::ZRANGE(Buffer key, int start, int end)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "ZRANGE %b %d %d", key.getData(), key.getLen(), start, end))
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("ZRANGE reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyTypeException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZRANGE type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(r, r->elements);
}

std::vector<std::pair<Buffer, Buffer>> 
Zset::ZRANGE_WITHSCORES(Buffer key, int start, int end)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "ZRANGE %b %d %d WITHSCORES", key.getData(), key.getLen(), start, end))
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("ZRANGE_WITHSCORES reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZRANGE_WITHSCORES type REDIS_REPLY_ARRAY");
    return replyArrayToPair(r, r->elements);
}

std::vector<Buffer> Zset::ZREVRANGE(Buffer key, int start, int end)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "ZREVRANGE %b %d %d", key.getData(), key.getLen(), start, end))
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("ZREVRANGE reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZREVRANGE type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(r, r->elements);
}

std::vector<std::pair<Buffer, Buffer>> 
Zset::ZREVRANGE_WITHSCORES(Buffer key, int start, int end)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "ZREVRANGE %b %d %d WITHSCORES", key.getData(), key.getLen(), start, end))
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("ZREVRANGE_WITHSCORES reply null");
    if (r->type == REDIS_REPLY_NIL)
        return{};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZREVRANGE_WITHSCORES type REDIS_REPLY_ARRAY");
    return replyArrayToPair(r, r->elements);
}

//在有序集合增加成员的分数
long long Zset::ZINCRBY(Buffer key, long long increment, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(), "ZINCRBY %b %lld %b", key.getData(), key.getLen(),
            increment, value.getData(), value.getLen()))
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("ZINCRBY reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("ZINCRBY type REDIS_REPLY_STRING");
    return replyToStringInt64(r->str);
}

}