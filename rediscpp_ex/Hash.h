#pragma once

#include <vector>

namespace rediscpp {

class Connection;
class Buffer;

class Hash
{
public:
    Hash(Connection& conn);
    ~Hash() = default;

    //�����е��ֶκ�ֵ��ָ���ļ��洢��һ����ϣ
    std::vector<std::pair<Buffer, Buffer>> HGETALL(Buffer key);

    //�ɸ����������ӵĹ�ϣ�ֶε�����ֵ
    long long HINCRBY(Buffer key, Buffer mkey, long long increment);

    //���ù�ϣ�ֶε��ַ���ֵ, return 0 1
    long long HSET(Buffer key, Buffer mkey, Buffer value);

    Buffer HGET(Buffer key, Buffer mkey);

    //ɾ��һ��������ϣ�ֶ�
    long long HDEL(Buffer key, Buffer mkey);
private:
    Connection& m_conn;
};

}