#include "RedisException.h"

namespace rediscpp {

RedisReplyExceiption::RedisReplyExceiption(const char* str)
{
    if (str)
        m_msg = str;
}

RedisReplyExceiption::RedisReplyExceiption(std::string str)
{
    m_msg = std::move(str);
}

RedisReplyTypeExceiption::RedisReplyTypeExceiption(const char* str)
{
    if (str)
        m_msg = str;
}

RedisReplyTypeExceiption::RedisReplyTypeExceiption(std::string str)
{
    m_msg = std::move(str);
}

}
