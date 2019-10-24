#pragma once

#include <cstdint>

typedef int64_t ActorID;
typedef int64_t ResultCode;

typedef void(*AcceptFunc)(int64_t server_index, int64_t index);
typedef void(*NetworkErrorFunc)(int64_t meta_index, int64_t index, int32_t type, int32_t error_code, const char* error_msg, const void* data, uint32_t data_length);
typedef void(*ConnectFunc)(int64_t id, void* context);


typedef void(*OnSendCallback)(ActorID id, const void* p, uint32_t length, void* ctx);
typedef void(*OnReceiveCallback)(ActorID id, const void* head, uint32_t head_length, const void* body, uint32_t body_length, void* ctx);

#define API_EXPORT __declspec(dllexport)

extern "C"
{
    API_EXPORT int Init();
    API_EXPORT int Cleanup();
    API_EXPORT ResultCode CreateClient(ActorID* out);
    API_EXPORT ResultCode Connect(const char* host, uint16_t port);
    API_EXPORT ResultCode AsyncSend(ActorID id, const void* p, uint32_t length);

    API_EXPORT ResultCode SetOnSend(ActorID id, OnSendCallback cb, void* ctx);
    API_EXPORT ResultCode SetOnReceive(ActorID id, OnReceiveCallback cb, void* ctx);

    API_EXPORT ResultCode Shutdown(ActorID id);
}
