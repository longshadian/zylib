#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace rediscpp {

//�쳣����
struct Exception : public std::runtime_error
{
    Exception(const std::string& s);
    Exception(const char* s);
    virtual ~Exception() throw () {}
};

//redis���񷵻��쳣
struct ReplyException : public Exception
{
    ReplyException(const std::string& s);
    ReplyException(const char* s);
    virtual ~ReplyException() throw () {}
};

//redis���������쳣
struct ConnectionException : public Exception
{
    ConnectionException(const std::string& s);
    ConnectionException(const char* s);
    virtual ~ConnectionException() throw () {}
};

//redis�������ݳ���
struct ReplyErrorException : public ReplyException
{
    ReplyErrorException(const std::string& s);
    ReplyErrorException(const char* str);
    virtual ~ReplyErrorException() throw () {}
};

//redis�������ͳ���
struct ReplyTypeException : public ReplyException
{
    ReplyTypeException(const std::string& s);
    ReplyTypeException(const char* str);
    virtual ~ReplyTypeException() throw () {}
};

}
