#pragma once

#include <vector>

namespace rediscpp {

class Buffer;
class Connection;

class Zset
{
public:
    Zset(Connection& context);
    ~Zset() = default;

    long long ZADD(Buffer key, long long score, Buffer value);

    //���򼯺Ϸ�Χ�ڵĳ�Ա�ͷ���
    std::vector<Buffer> ZRANGE(Buffer key, int start, int end);
    std::vector<std::pair<Buffer, Buffer>> ZRANGE_WITHSCORES(Buffer key, int start, int end);

    //���򼯺Ϸ�Χ�ڵĳ�Ա�ͷ���,����
    //first:    value
    //second:   score
    std::vector<Buffer> ZREVRANGE(Buffer key, int start, int end);
    std::vector<std::pair<Buffer, Buffer>> ZREVRANGE_WITHSCORES(Buffer key, int start, int end);

    //�����򼯺����ӳ�Ա�ķ���
    long long ZINCRBY(Buffer key, long long increment, Buffer value);
private:
    Connection& m_context;
};

}