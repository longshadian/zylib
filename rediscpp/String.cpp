#include "String.h"

#include "Exception.h"
#include "Utile.h"
#include "Buffer.h"
#include "Connection.h"

namespace rediscpp {

String::String(Connection& conn)
    : m_conn(conn)
{}

void String::SET(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SET %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SET reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("SET type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("SET type REDIS_REPLY_STATUS != OK");
}

Buffer String::GET(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"GET %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("GET reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("GET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

void String::GETRANGE(Buffer key, long long start, long long end)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"GETRANGE %b %lld %lld", key.getData(), key.getLen(), start, end)
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("GETRANGE reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("GETRANGE type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("GETRANGE type REDIS_REPLY_STATUS != OK");
}

Buffer String::GETSET(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"GETSET %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("GETSET reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("GETSET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
} 

Buffer String::SETEX(Buffer key, long long timeout, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SETEX %b %lld %b", key.getData(), key.getLen(),
            timeout, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SETEX reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("SETEX type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

bool String::SETNX(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SETNX %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SETNX reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SETNX type REDIS_REPLY_INTEGER");
    return r->integer == 1;
}

Buffer String::SETRANGE(Buffer key, long long offset, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SETRANGE %b %lld %b", key.getData(), key.getLen(), offset, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SETRANGE reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("SETRANGE type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

long long String::STRLEN(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"STRLEN %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("STRLEN reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("STRLEN type REDIS_REPLY_INTEGER");
    return r->integer;
}

void String::PSETEX(Buffer key, long long timeout, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"PSETEX %b %lld %b", key.getData(), key.getLen(), timeout, value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("PSETEX reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STATUS)
        throw ReplyTypeException("PSETEX type REDIS_REPLY_STATUS");
    if (r->str != std::string("OK"))
        throw ReplyTypeException("PSETEX type REDIS_REPLY_STATUS != OK");
}

long long String::INCR(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"INCR %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("INCR reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("INCR type REDIS_REPLY_INTEGER");
    return r->integer;
}

long long String::INCRBY(Buffer key, long long increment)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"INCRBY %b %lld", key.getData(), key.getLen(), increment)
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("INCRBY reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("INCRBY type REDIS_REPLY_INTEGER");
    return r->integer;
}

Buffer String::INCRBYFLOAT(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"INCRBYFLOAT %b %b", key.getData(), key.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("INCRBYFLOAT reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("INCRBYFLOAT type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

}