#include "RedisZset.h"

#include "RedisException.h"
#include "Utile.h"

namespace rediscpp {

RedisZset::RedisZset(ContextGuard& context)
    : m_context(context)
{}

long long RedisZset::ZADD(Buffer key, long long score, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"ZADD %b %lld %b", key.getData(), key.getLen(),
            score, value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("ZADD reply null");
    if (reply->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("ZADD REDIS_REPLY_INTEGER");
    return reply->integer;
}

std::vector<Buffer> RedisZset::ZRANGE(Buffer key, int start, int end)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(), "ZRANGE %b %d %d", key.getData(), key.getLen(), start, end))
    };
    if (!reply)
        throw ReplyNullException("ZRANGE reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyTypeException(reply->str);
    if (reply->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZRANGE type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(reply.get(), reply->elements);
}

std::vector<std::pair<Buffer, Buffer>> 
RedisZset::ZRANGE_WITHSCORES(Buffer key, int start, int end)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(), "ZRANGE %b %d %d WITHSCORES", key.getData(), key.getLen(), start, end))
    };
    if (!reply)
        throw ReplyNullException("ZRANGE_WITHSCORES reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZRANGE_WITHSCORES type REDIS_REPLY_ARRAY");
    return replyArrayToPair(reply.get(), reply->elements);
}

std::vector<Buffer> RedisZset::ZREVRANGE(Buffer key, int start, int end)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(), "ZREVRANGE %b %d %d", key.getData(), key.getLen(), start, end))
    };
    if (!reply)
        throw ReplyNullException("ZREVRANGE reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZREVRANGE type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(reply.get(), reply->elements);
}

std::vector<std::pair<Buffer, Buffer>> 
RedisZset::ZREVRANGE_WITHSCORES(Buffer key, int start, int end)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(), "ZREVRANGE %b %d %d WITHSCORES", key.getData(), key.getLen(), start, end))
    };
    if (!reply)
        throw ReplyNullException("ZREVRANGE_WITHSCORES reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return{};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("ZREVRANGE_WITHSCORES type REDIS_REPLY_ARRAY");
    return replyArrayToPair(reply.get(), reply->elements);
}

//在有序集合增加成员的分数
long long RedisZset::ZINCRBY(Buffer key, long long increment, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(), "ZINCRBY %b %lld %b", key.getData(), key.getLen(),
            increment, value.getData(), value.getLen()))
    };
    if (!reply)
        throw ReplyNullException("ZINCRBY reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("ZINCRBY type REDIS_REPLY_STRING");
    return replyToStringInt64(reply->str);
}

}