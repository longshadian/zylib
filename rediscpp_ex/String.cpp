#include "String.h"

#include "Exception.h"
#include "Utile.h"
#include "Buffer.h"
#include "Connection.h"

namespace rediscpp {

RedisString::RedisString(Connection& context)
    : m_context(context)
{}

void RedisString::SET(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"SET %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("SET reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("SET type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("SET type REDIS_REPLY_STATUS != OK");
}

Buffer RedisString::GET(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"GET %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("GET reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("GET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

void RedisString::GETRANGE(Buffer key, long long start, long long end)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"GETRANGE %b %lld %lld", key.getData(), key.getLen(), start, end)
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("GETRANGE reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("GETRANGE type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("GETRANGE type REDIS_REPLY_STATUS != OK");
}

Buffer RedisString::GETSET(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"GETSET %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("GETSET reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("GETSET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
} 

Buffer RedisString::SETEX(Buffer key, long long timeout, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"SETEX %b %lld %b", key.getData(), key.getLen(),
            timeout, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("SETEX reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("SETEX type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

bool RedisString::SETNX(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"SETNX %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("SETNX reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SETNX type REDIS_REPLY_INTEGER");
    return r->integer == 1;
}

Buffer RedisString::SETRANGE(Buffer key, long long offset, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"SETRANGE %b %lld %b", key.getData(), key.getLen(), offset, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("SETRANGE reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("SETRANGE type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

long long RedisString::STRLEN(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"STRLEN %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("STRLEN reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("STRLEN type REDIS_REPLY_INTEGER");
    return r->integer;
}

void RedisString::PSETEX(Buffer key, long long timeout, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"PSETEX %b %lld %b", key.getData(), key.getLen(), timeout, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("PSETEX reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("PSETEX type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("PSETEX type REDIS_REPLY_STATUS != OK");
}

long long RedisString::INCR(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"INCR %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("INCR reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("INCR type REDIS_REPLY_INTEGER");
    return r->integer;
}

long long RedisString::INCRBY(Buffer key, long long increment)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"INCRBY %b %lld", key.getData(), key.getLen(), increment)
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("INCRBY reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("INCRBY type REDIS_REPLY_INTEGER");
    return r->integer;
}

Buffer RedisString::INCRBYFLOAT(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.getRedisContext(),"INCRBYFLOAT %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ReplyNullException("INCRBYFLOAT reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("INCRBYFLOAT type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

}