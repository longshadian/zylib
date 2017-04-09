#pragma once

#include <hiredis.h>

#include <string>
#include <vector>
#include <memory>

namespace rediscpp {

class Reply
{
public:
    Reply();
    Reply(redisReply* redis_reply);
    ~Reply();

    Reply(const Reply& rhs) = delete;
    Reply& operator=(const Reply& rhs) = delete;

    Reply(Reply&& rhs);
    Reply& operator=(Reply&& rhs);

public:
    operator bool() const;
    redisReply* getRedisReply();
private:
    redisReply* m_redis_reply;
};


class Connection
{
public:
    Connection();
    Connection(redisContext* redis_context);
    ~Connection();

    Connection(const Connection& rhs) = delete; 
    Connection& operator=(const Connection& rhs) = delete;

    Connection(Connection&& rhs);
    Connection& operator=(Connection&& rhs);
  
public:
    operator bool() const;
    redisContext* getRedisContext();

private:
    redisContext* m_redis_context;
}

}
