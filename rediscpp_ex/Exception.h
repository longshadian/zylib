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

//redis�����û�з���
struct ReplyNullException : public RedisException
{
    ReplyNullException(const std::string& s);
    ReplyNullException(const char* s);
    virtual ~ReplyNullException() throw () {}
};

//redis�������ݳ���
struct ReplyErrorException : public RedisException
{
    ReplyErrorException(const std::string& s);
    ReplyErrorException(const char* str);
    virtual ~ReplyErrorException() throw () {}
};

//redis�������ͳ���
struct ReplyTypeException : public RedisException
{
    ReplyTypeException(const std::string& s);
    ReplyTypeException(const char* str);
    virtual ~ReplyTypeException() throw () {}
};

}
