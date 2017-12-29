#pragma once

#include <cstdint>
#include <chrono>
#include <memory>

#include "net/NetworkType.h"
#include "net/Message.h"

class FESCallback : public ServerCallback
{
public:
    FESCallback() = default;
    virtual ~FESCallback() = default;
    FESCallback(const FESCallback& rhs) = delete;
    FESCallback& operator=(const FESCallback& rhs) = delete;
    FESCallback(FESCallback&& rhs) = delete;
    FESCallback& operator=(FESCallback&& rhs) = delete;

    // 新的handler创建了
    virtual void HandlerAccept(Hdl hdl) override;

    // handler关闭
    virtual void HandlerClosed(Hdl hdl) override;

    // handler超时
    virtual void HandlerTimeout(Hdl hdl) override;

    // handler收到消息
    virtual void ReceviedMessage(Hdl hdl, std::shared_ptr<CSMessage> msg) override;

    // server可以得accept的handler超出上限
    virtual void HandlerAcceptOverflow() override;
};

