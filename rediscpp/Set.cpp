#include "Set.h"

#include "Exception.h"
#include "Utile.h"
#include "Connection.h"
#include "Buffer.h"

namespace rediscpp {

Set::Set(Connection& conn)
    : m_conn(conn)
{}

long long Set::SADD(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SADD %b %b", key.getData(), key.getLen(),
            value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SADD reply null");
    if (r->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SADD type REDIS_REPLY_INTEGER");
    return r->integer;
}

bool Set::SISMEMBER(Buffer key, Buffer value)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SISMEMBER %b %b", key.getData(), key.getLen(),
            value.getData(), value.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SISMEMBER reply null");
    if (r->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SISMEMBER type REDIS_REPLY_INTEGER");
    return r->integer == 1;
}

long long Set::SCARD(Buffer key)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SCARD %b", key.getData(), key.getLen())
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SCARD reply null");
    if (r->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(r->str);
    if (r->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SCARD type REDIS_REPLY_INTEGER");
    return r->integer;
}

std::vector<Buffer> Set::SRANDMEMBER(Buffer key, int32_t len)
{
    Reply reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_conn.getRedisContext(),"SRANDMEMBER %b %d", key.getData(), key.getLen(), len)
        )
    };
    redisReply* r = reply.getRedisReply();
    if (!r)
        throw ConnectionException("SRANDMEMBER reply null");
    if (r->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(r->str);
    if (r->type == REDIS_REPLY_NIL)
        return {};
    if (r->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("SRANDMEMBER type REDIS_REPLY_ARRAY");
    return replyArrayToBuffer(r, r->elements);
}

}