#include "DllDefine.h"

#include "GlobalInstance.h"

extern "C"
{
    API_EXPORT int Init()
    {
        GlobalInstance::Get()->Init();
        return 0;
    }

    API_EXPORT int Cleanup()
    {
        return 0;
    }

    API_EXPORT ResultCode CreateClient(ActorID* out)
    {
        return GlobalInstance::Get()->CreateClient(out);
    }

    API_EXPORT ResultCode Connect(ActorID id, const char* host, uint16_t port)
    {
        return GlobalInstance::Get()->Connect(id, host, port);
    }

    API_EXPORT ResultCode AsyncSend(ActorID id, const void* p, uint32_t length)
    {

    }

    API_EXPORT ResultCode SetOnSend(ActorID id, OnSendCallback cb, void* ctx)
    {

    }

    API_EXPORT ResultCode SetOnReceive(ActorID id, OnReceiveCallback cb, void* ctx)
    {

    }

    API_EXPORT ResultCode Shutdown(ActorID id)
    {

    }
}
