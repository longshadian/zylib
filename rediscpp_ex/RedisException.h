#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace rediscpp {

struct RedisException : public std::runtime_error
{
    RedisException(const std::string& s);
    RedisException(const char* s);
    virtual ~RedisException() throw () {}
};

//redis服务端没有返回
struct ReplyNullException : public RedisException
{
    ReplyNullException(const std::string& s);
    ReplyNullException(const char* s);
    virtual ~ReplyNullException() throw () {}
};

//redis返回内容出错
struct ReplyErrorException : public RedisException
{
    ReplyErrorException(const std::string& s);
    ReplyErrorException(const char* str);
    virtual ~ReplyErrorException() throw () {}
};

//redis返回类型出错
struct ReplyTypeException : public RedisException
{
    ReplyTypeException(const std::string& s);
    ReplyTypeException(const char* str);
    virtual ~ReplyTypeException() throw () {}
};

}
