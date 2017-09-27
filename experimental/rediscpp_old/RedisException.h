#pragma once

#include <exception>
#include <string>

namespace rediscpp {

class RedisException : public std::exception
{
public:
    ~RedisException() throw() {}

    virtual const char* what() const noexcept override { return m_msg.c_str(); }
protected:
    std::string m_msg;
};

class RedisReplyExceiption : public RedisException
{
public:
    RedisReplyExceiption(const char* str);
    RedisReplyExceiption(std::string str);
    ~RedisReplyExceiption() throw() {}
};

class RedisReplyTypeExceiption : public RedisException
{
public:
    RedisReplyTypeExceiption(const char* str);
    RedisReplyTypeExceiption(std::string str);
    ~RedisReplyTypeExceiption() throw() {}
};


}