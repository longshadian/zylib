#include "Hash.h"

#include "Exception.h"
#include "Utile.h"
#include "Buffer.h"
#include "Connection.h"

namespace rediscpp {

Hash::Hash(Connection& conn)
    : m_conn(conn)
{
}


std::vector<std::pair<Buffer, Buffer>> Hash::HGETALL(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"HGETALL %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("HGETALL reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("HGETALL type REDIS_REPLY_ARRAY");
    return replyArrayToPair(r, r->elements);
}

//由给定数量增加的哈希字段的整数值
long long Hash::HINCRBY(Buffer key, Buffer mkey, long long increment)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"HINCRBY %b %b %lld", key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen(), increment)
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("HINCRBY reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("HINCRBY type REDIS_REPLY_INTEGER");
    return r->integer;
}

//设置哈希字段的字符串值, return 0 1
long long Hash::HSET(Buffer key, Buffer mkey, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"HSET %b %b %b", key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen(), value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("HSET reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("HSET type REDIS_REPLY_INTEGER");
    return r->integer;
}

Buffer Hash::HGET(Buffer key, Buffer mkey)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(), "HGET %b %b",
            key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("HGET reply null");
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("HGET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(r);
}

long long Hash::HDEL(Buffer key, Buffer mkey)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"HDEL %b %b", key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("HDEL reply null");
    if (r->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("HDEL type REDIS_REPLY_INTEGER");
    return r->integer;
}

}