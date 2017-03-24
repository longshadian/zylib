#include "RedisException.h"

namespace rediscpp {

RedisException::RedisException(const std::string& s)
    : std::runtime_error(s)
{
}

RedisException::RedisException(const char* s)
    : std::runtime_error(s)
{
}


ReplyNullException::ReplyNullException(const std::string& s)
    : RedisException(s)
{
}

ReplyNullException::ReplyNullException(const char* s)
    : RedisException(s)
{
}


ReplyErrorException::ReplyErrorException(const std::string& s)
    : RedisException(s)
{
}

ReplyErrorException::ReplyErrorException(const char* str)
    : RedisException(str)
{
}

ReplyTypeException::ReplyTypeException(const char* str)
    : RedisException(str)
{
}

ReplyTypeException::ReplyTypeException(const std::string& str)
    : RedisException(str)
{
}

}
