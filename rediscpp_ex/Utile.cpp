#include "Utile.h"

#include <sstream>
#include <fstream>
#include "RedisException.h"

namespace rediscpp {

ContextGuard redisConnect(std::string ip, int port)
{
    return ContextGuard(::redisConnect(ip.c_str(), port));
}

ContextGuard redisConnectWithTimeout(std::string ip, int port, int seconds, int microseconds)
{
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;
    return ContextGuard(::redisConnectWithTimeout(ip.c_str(), port, tv));
}

int redisGetReply(redisContext* context, ReplyGuard* guard)
{
    redisReply* reply = nullptr;
    int ret = ::redisGetReply(context, (void**)&reply);
    if (ret != REDIS_OK)
        return ret;
    *guard = ReplyGuard(reply);
    return ret;
}

std::string replyToString(const void* p)
{
    auto reply = reinterpret_cast<const redisReply*>(p);
    std::string s(reply->str, reply->len);
    return s;
}

int32_t replyToStringInteger(const void* p)
{
    auto s = replyToString(p);
    return atoi(s.c_str());
}

uint64_t replyToStringUint64(const char* p)
{
    return static_cast<uint64_t>(std::strtoull((const char*)p, nullptr, 10));
}

int64_t replyToStringInt64(const char* p)
{
    return static_cast<int64_t>(std::strtoll((const char*)p, nullptr, 10));
}

Buffer replyToRedisBuffer(const void* p)
{
    auto reply = reinterpret_cast<const redisReply*>(p);
    return Buffer(reply->str, reply->len);
}

std::vector<std::pair<Buffer, Buffer>> replyArrayToPair(const redisReply* reply, size_t count)
{
    if (count % 2 != 0) {
        throw RedisException("replyArrayToPair count error");
    }

    std::vector<std::pair<Buffer, Buffer>> ret;
    for (size_t i = 0; i != count; ++i) {
        Buffer key = replyToRedisBuffer(reply->element[i]);
        ++i;
        if (i >= count) {
            throw RedisException("Array count error");
        }
        Buffer value = replyToRedisBuffer(reply->element[i]);
        ret.emplace_back(std::make_pair(key, value));
    }
    return ret;
}

std::vector<Buffer> replyArrayToBuffer(const redisReply* reply, size_t count)
{
    std::vector<Buffer> ret;
    for (size_t i = 0; i != count; ++i) {
        ret.emplace_back(replyToRedisBuffer(reply->element[i]));
    }
    return ret;
}

long long DEL(ContextGuard& context, std::string key)
{
    std::vector<std::string> temp;
    temp.emplace_back(std::move(key));
    return DEL(context, temp);
}

long long DEL(ContextGuard& context, std::vector<std::string> keys)
{
    std::vector<Buffer> temp;
    for (const auto& it : keys) {
        temp.emplace_back(Buffer(it));
    }
    return DEL(context, temp);
}

long long DEL(ContextGuard& context, Buffer key)
{
    std::vector<Buffer> temp;
    temp.emplace_back(std::move(key));
    return DEL(context, std::move(temp));
}

long long DEL(ContextGuard& context, std::vector<Buffer> keys)
{
    if (keys.empty()) {
        return 0;
    }

    std::ostringstream ostm;
    ostm << "DEL ";
    for (const auto& it : keys) {
        ostm << ' ' << it.asString();
    }

    std::string cmd = ostm.str();
    ReplyGuard reply{ reinterpret_cast<redisReply*>(::redisCommand(context.get(), cmd.c_str()))};
    if (!reply)
        throw ReplyNullException("DEL reply null");
    if (reply->type == REDIS_REPLY_ERROR)
        throw ReplyErrorException(reply->str);
    if (reply->type != REDIS_REPLY_INTEGER)
        throw ReplyTypeException("DEL type REDIS_REPLY_INTEGER");
    return reply->integer;
}

std::string catFile(std::string path)
{
    std::string context;
    std::ifstream ifsm(path);
    if (!ifsm)
        return {};
    auto a = ifsm.get();
    while (a != EOF) {
        context.push_back(static_cast<char>(a));
        a = ifsm.get();
    }
    return context;
}

}
