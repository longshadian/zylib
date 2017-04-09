#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace rediscpp {

//异常基类
struct Exception : public std::runtime_error
{
    Exception(const std::string& s);
    Exception(const char* s);
    virtual ~Exception() throw () {}
};

//redis服务返回异常
struct ReplyException : public Exception
{
    ReplyException(const std::string& s);
    ReplyException(const char* s);
    virtual ~ReplyException() throw () {}
};

//redis服务连接异常
struct ConnectionException : public Exception
{
    ConnectionException(const std::string& s);
    ConnectionException(const char* s);
    virtual ~ConnectionException() throw () {}
};

//redis返回内容出错
struct ReplyErrorException : public ReplyException
{
    ReplyErrorException(const std::string& s);
    ReplyErrorException(const char* str);
    virtual ~ReplyErrorException() throw () {}
};

//redis返回类型出错
struct ReplyTypeException : public ReplyException
{
    ReplyTypeException(const std::string& s);
    ReplyTypeException(const char* str);
    virtual ~ReplyTypeException() throw () {}
};

}
