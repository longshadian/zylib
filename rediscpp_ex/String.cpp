#include "RedisString.h"

#include "RedisException.h"
#include "Utile.h"

namespace rediscpp {

RedisString::RedisString(ContextGuard& context)
    : m_context(context)
{}

void RedisString::SET(Buffer key, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"SET %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("SET reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("SET type REDIS_REPLY_STATUS");
    if (reply->str != std::string("OK"))
        throw ReplyTypeException("SET type REDIS_REPLY_STATUS != OK");
}

Buffer RedisString::GET(Buffer key)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"GET %b", key.getData(), key.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("GET reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("GET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(reply.get());
}

void RedisString::GETRANGE(Buffer key, long long start, long long end)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"GETRANGE %b %lld %lld", key.getData(), key.getLen(), start, end)
        )
    };
    if (!reply)
        throw ReplyNullException("GETRANGE reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("GETRANGE type REDIS_REPLY_STATUS");
    if (reply->str != std::string("OK"))
        throw ReplyTypeException("GETRANGE type REDIS_REPLY_STATUS != OK");
}

Buffer RedisString::GETSET(Buffer key, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"GETSET %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("GETSET reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("GETSET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(reply.get());
} 

Buffer RedisString::SETEX(Buffer key, long long timeout, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"SETEX %b %lld %b", key.getData(), key.getLen(),
            timeout, value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("SETEX reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("SETEX type REDIS_REPLY_STRING");
    return replyToRedisBuffer(reply.get());
}

bool RedisString::SETNX(Buffer key, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"SETNX %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("SETNX reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SETNX type REDIS_REPLY_INTEGER");
    return reply->integer == 1;
}

Buffer RedisString::SETRANGE(Buffer key, long long offset, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"SETRANGE %b %lld %b", key.getData(), key.getLen(), offset, value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("SETRANGE reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("SETRANGE type REDIS_REPLY_STRING");
    return replyToRedisBuffer(reply.get());
}

long long RedisString::STRLEN(Buffer key)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"STRLEN %b", key.getData(), key.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("STRLEN reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("STRLEN type REDIS_REPLY_INTEGER");
    return reply->integer;
}

void RedisString::PSETEX(Buffer key, long long timeout, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"PSETEX %b %lld %b", key.getData(), key.getLen(), timeout, value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("PSETEX reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("PSETEX type REDIS_REPLY_STATUS");
    if (reply->str != std::string("OK"))
        throw ReplyTypeException("PSETEX type REDIS_REPLY_STATUS != OK");
}

long long RedisString::INCR(Buffer key)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"INCR %b", key.getData(), key.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("INCR reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("INCR type REDIS_REPLY_INTEGER");
    return reply->integer;
}

long long RedisString::INCRBY(Buffer key, long long increment)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"INCRBY %b %lld", key.getData(), key.getLen(), increment)
        )
    };
    if (!reply)
        throw ReplyNullException("INCRBY reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("INCRBY type REDIS_REPLY_INTEGER");
    return reply->integer;
}

Buffer RedisString::INCRBYFLOAT(Buffer key, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
    ::redisCommand(m_context.get(),"INCRBYFLOAT %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
    )
    };
    if (!reply)
        throw ReplyNullException("INCRBYFLOAT reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("INCRBYFLOAT type REDIS_REPLY_STRING");
    return replyToRedisBuffer(reply.get());
}

}