#pragma once

#include <vector>


namespace rediscpp {

class Connection;
class Buffer;

class RedisList 
{
public:
    RedisList(Connection& context);
    ~RedisList() = default;
public:

    //��ȡ�б�ĳ���
    long long LLEN(Buffer key);

    //��ȡ��ȡ���б��еĵ�һ��Ԫ��
    Buffer LPOP(Buffer key);
    Buffer RPOP(Buffer key);

    //��һ���б��ȡ����Ԫ��
    std::vector<Buffer> LRANGE(Buffer key, int start, int stop);

    //���ص�ǰ�б���
    long long LPUSH(Buffer key, Buffer val);
    long long RPUSH(Buffer key, Buffer val);
private:
    Connection& m_context;
};

}