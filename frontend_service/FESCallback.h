#pragma once

#include <cstdint>
#include <chrono>
#include <memory>

#include "net/NetworkType.h"
#include "net/Message.h"

class FESCallback : public network::ServerCallback
{
public:
    FESCallback() = default
    virtual ~FESCallback() = default;
    FESCallback(const FESCallback& rhs) = delete;
    FESCallback& operator=(const FESCallback& rhs) = delete;
    FESCallback(FESCallback&& rhs) = delete;
    FESCallback& operator=(FESCallback&& rhs) = delete;

    // 新的handler创建了
    virtual void HandlerAccept(network::Hdl hdl);

    // handler关闭
    virtual void HandlerClosed(Hdl hdl);

    // handler超时
    virtual void HandlerTimeout(Hdl hdl);

    // handler收到消息
    virtual void ReceviedMessage(Hdl hdl, std::shared_ptr<Message> msg);

    // server可以得accept的handler超出上限
    virtual void HandlerAcceptOverflow();
};

