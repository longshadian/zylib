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

//redis�����û�з���
struct ReplyNullException : public Exception
{
    ReplyNullException(const std::string& s);
    ReplyNullException(const char* s);
    virtual ~ReplyNullException() throw () {}
};

//redis�������ݳ���
struct ReplyErrorException : public Exception
{
    ReplyErrorException(const std::string& s);
    ReplyErrorException(const char* str);
    virtual ~ReplyErrorException() throw () {}
};

//redis�������ͳ���
struct ReplyTypeException : public Exception
{
    ReplyTypeException(const std::string& s);
    ReplyTypeException(const char* str);
    virtual ~ReplyTypeException() throw () {}
};

}
