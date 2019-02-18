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

    //获取列表的长度
    long long LLEN(Buffer key);

    //获取并取出列表中的第一个元素
    Buffer LPOP(Buffer key);
    Buffer RPOP(Buffer key);

    //从一个列表获取各种元素
    std::vector<Buffer> LRANGE(Buffer key, int start, int stop);

    //返回当前列表长度
    long long LPUSH(Buffer key, Buffer val);
    long long RPUSH(Buffer key, Buffer val);

    void LTRIM(Buffer key, int start, int stop);
    Buffer LINDEX(Buffer key, int index);
private:
    Connection& m_conn;
};

}