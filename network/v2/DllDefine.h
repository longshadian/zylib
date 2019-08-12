#pragma once

#include <cstdint>

enum EShutdownType
{
    ConnectError    = 0,
    AcceptError,
    SendError,
    ReceiveError,

    SendTimeout     = 100,
    ReceiveTimeout  = 101,
};

enum EErrorCode
{
    HeadLengthTooShort  = -1000,
    HeadLengthTooLong   = -1001,
};

typedef void(*AcceptFunc)(std::int64_t server_index, std::int64_t index);
typedef void(*ConnectFunc)(std::int64_t client_index);
typedef void(*SendFunc)(std::int64_t meta_index, std::int64_t index, const void* data, std::uint32_t length);
typedef void(*ReceiveFunc)(std::int64_t meta_index, std::int64_t index, const void* head, std::uint32_t head_length, const void* body, std::uint32_t body_length);
typedef void(*NetworkErrorFunc)(std::int64_t meta_index, std::int64_t index, std::int32_t type, std::int32_t error_code, const char* error_msg, const void* data, std::uint32_t data_length);

