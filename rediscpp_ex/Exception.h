#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace rediscpp {

struct Exception : public std::runtime_error
{
    Exception(const std::string& s);
    Exception(const char* s);
    virtual ~Exception() throw () {}
};

//redis服务端没有返回
struct ReplyNullException : public Exception
{
    ReplyNullException(const std::string& s);
    ReplyNullException(const char* s);
    virtual ~ReplyNullException() throw () {}
};

//redis返回内容出错
struct ReplyErrorException : public Exception
{
    ReplyErrorException(const std::string& s);
    ReplyErrorException(const char* str);
    virtual ~ReplyErrorException() throw () {}
};

//redis返回类型出错
struct ReplyTypeException : public Exception
{
    ReplyTypeException(const std::string& s);
    ReplyTypeException(const char* str);
    virtual ~ReplyTypeException() throw () {}
};

}
