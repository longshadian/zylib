#pragma once

#include "RedisType.h"

namespace rediscpp {

class RedisString
{
public:
    RedisString(ContextGuard& context);
    ~RedisString() = default;

    void SET(Buffer key, Buffer value);
    Buffer GET(Buffer key);
    void GETRANGE(Buffer key, long long start, long long end);
    Buffer GETSET(Buffer key, Buffer value);
    bool SETNX(Buffer key, Buffer value);
    Buffer SETRANGE(Buffer key, long long offset, Buffer value);
    long long STRLEN(Buffer key);
    Buffer SETEX(Buffer key, long long timeout, Buffer value);
    void PSETEX(Buffer key, long long timeout, Buffer value);
    long long INCR(Buffer key);
    long long INCRBY(Buffer key, long long increment);
    Buffer INCRBYFLOAT(Buffer key, Buffer value);
private:
    ContextGuard& m_context;
};

}