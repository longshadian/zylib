#pragma once

#include "RedisType.h"

namespace rediscpp {

class RedisList 
{
public:
    RedisList(RedisContextGuard& context);
    ~RedisList() = default;

    //��ȡ�б�ĳ���
    long long LLEN(Buffer key);

    Buffer LINDEX(Buffer key, int idx);

    //��ȡ��ȡ���б��еĵ�һ��Ԫ��
    Buffer LPOP(Buffer key);
    Buffer RPOP(Buffer key);

    //��һ���б��ȡ����Ԫ��
    std::vector<Buffer> LRANGE(Buffer key, int start, int stop);

    //���ص�ǰ�б���
    long long LPUSH(Buffer key, Buffer val);
    long long RPUSH(Buffer key, Buffer val);
private:
    RedisContextGuard& m_context;
};

}