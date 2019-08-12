#pragma once

#include <cstdint>
#include "network/Event.h"
#include "DllDefine.h"
#include "GlobalInstance.h"

#define DLL_EXPORT _declspec(dllexport)

extern "C"
{
    DLL_EXPORT void Init(const char* host, std::int16_t port)
    {
        GlobalInstance::Get()->Init();
    }

    DLL_EXPORT void Cleanup(const char* host, std::int16_t port)
    {
        GlobalInstance::Get()->Cleanup();
    }

    DLL_EXPORT std::int64_t CreateTcpServer(const char* host, std::int16_t port)
    {
        return GlobalInstance::Get()->CreateServer(host, port);
    }

    DLL_EXPORT std::int64_t CreateTcpClient(const char* host, std::int16_t port)
    {
        return GlobalInstance::Get()->CreateClient(host, port);
    }

    DLL_EXPORT std::int32_t TcpLaunch(std::int64_t index)
    {
        bool succ = GlobalInstance::Get()->TcpLaunch(index);
        return succ ? 1 : 0;
    }

    DLL_EXPORT void SetAcceptFunc(AcceptFunc cb)
    {
        GlobalInstance::Get()->m_accect_func = cb;
    }

    DLL_EXPORT void SetConnectFunc(ConnectFunc cb)
    {
        GlobalInstance::Get()->m_connect_func = cb;
    }

    DLL_EXPORT void SetSendFunc(SendFunc cb)
    {
        GlobalInstance::Get()->m_send_func = cb;
    }

    DLL_EXPORT void SetReceiveFunc(ReceiveFunc cb)
    {
        GlobalInstance::Get()->m_receive_func = cb;
    }

    DLL_EXPORT void SetNetworkErrorFunc(NetworkErrorFunc cb)
    {
        GlobalInstance::Get()->m_network_error_func = cb;
    }

}

/*
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        boost::thread mythread1(ThreadBeginWork);	//开始工作
    }
    break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        //CNetWorkSingleton::Instance()->Exit();
        //CSingletonWorkPool::Instance()->Uninit();
        break;
    }
    return TRUE;
}
*/
