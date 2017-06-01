#pragma once

#include <sys/time.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <hiredis.h>


namespace rediscpp {

class Connection;
class Reply;
class Buffer;
class BufferArray;

Connection redisConnect(std::string ip, int port);
Connection redisConnectWithTimeout(std::string ip, int port, struct timeval tv);
int redisGetReply(redisContext* context, Reply* guard);

std::string replyToString(const void* p);
int32_t replyToStringInteger(const void* p);

uint64_t replyToStringUint64(const char* p);
int64_t replyToStringInt64(const char* p);

Buffer replyToRedisBuffer(const void* p);

std::vector<std::pair<Buffer, Buffer>> replyArrayToPair(const redisReply* reply, size_t count);
std::vector<Buffer> replyArrayToBuffer(const redisReply* reply, size_t count);

long long DEL(Connection& conn, std::string key);
long long DEL(Connection& conn, std::vector<std::string> keys);
long long DEL(Connection& conn, Buffer key);
long long DEL(Connection& conn, std::vector<Buffer> keys);

std::vector<Buffer> KEYS(Connection& conn, Buffer key);

std::string catFile(std::string path);
}
