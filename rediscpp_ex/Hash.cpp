#include "RedisHash.h"

#include "RedisException.h"
#include "Utile.h"

namespace rediscpp {

RedisHash::RedisHash(ContextGuard& context)
    : m_context(context)
{

}

std::vector<std::pair<Buffer, Buffer>> RedisHash::HGETALL(Buffer key)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"HGETALL %b", key.getData(), key.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("HGETALL reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_ARRAY)
        throw ReplyTypeException("HGETALL type REDIS_REPLY_ARRAY");
    return replyArrayToPair(reply.get(), reply->elements);
}

//�ɸ����������ӵĹ�ϣ�ֶε�����ֵ
long long RedisHash::HINCRBY(Buffer key, Buffer mkey, long long increment)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"HINCRBY %b %b %lld", key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen(), increment)
        )
    };
    if (!reply)
        throw ReplyNullException("HINCRBY reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("HINCRBY type REDIS_REPLY_INTEGER");
    return reply->integer;
}

//���ù�ϣ�ֶε��ַ���ֵ, return 0 1
long long RedisHash::HSET(Buffer key, Buffer mkey, Buffer value)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"HSET %b %b %b", key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen(), value.getData(), value.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("HSET reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("HSET type REDIS_REPLY_INTEGER");
    return reply->integer;
}

Buffer RedisHash::HGET(Buffer key, Buffer mkey)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(), "HGET %b %b",
            key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("HGET reply null");
    if (reply->type == REDIS_REPLY_NIL)
        return {};
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_STRING)
        throw ReplyTypeException("HGET type REDIS_REPLY_STRING");
    return replyToRedisBuffer(reply.get());
}

long long RedisHash::HDEL(Buffer key, Buffer mkey)
{
    ReplyGuard reply{ reinterpret_cast<redisReply*>(
        ::redisCommand(m_context.get(),"HDEL %b %b", key.getData(), key.getLen(),
            mkey.getData(), mkey.getLen())
        )
    };
    if (!reply)
        throw ReplyNullException("HDEL reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("HDEL type REDIS_REPLY_INTEGER");
    return reply->integer;
}

}