#pragma once

#include <vector>


namespace rediscpp {

class Connection;
class Buffer;

class List 
{
public:
    List(Connection& conn);
    ~List() = default;
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

    void LTRIM(Buffer key, int start, int stop);
    Buffer LINDEX(Buffer key, int index);
private:
    Connection& m_conn;
};

}