#include "RedisSet.h"

#include "RedisException.h"
#include "Utile.h"

namespace rediscpp {

Set::Set(ContextGuard& context)
    : m_context(context)
{}

long long Set::SADD(Buffer key, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"SADD %b %b", key.getData(), key.getLen(),
            value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("SADD reply null");
    if (reply->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SADD type REDIS_REPLY_INTEGER");
    return reply->integer;
}

bool Set::SISMEMBER(Buffer key, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"SISMEMBER %b %b", key.getData(), key.getLen(),
            value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("SISMEMBER reply null");
    if (reply->type == REDIS_REPLY_ERROR) 
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("SISMEMBER type REDIS_REPLY_INTEGER");
    return reply->integer == 1;
}

}