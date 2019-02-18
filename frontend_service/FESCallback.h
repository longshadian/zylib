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

    // �µ�handler������
    virtual void HandlerAccept(Hdl hdl) override;

    // handler�ر�
    virtual void HandlerClosed(Hdl hdl) override;

    // handler��ʱ
    virtual void HandlerTimeout(Hdl hdl) override;

    // handler�յ���Ϣ
    virtual void ReceviedMessage(Hdl hdl, std::shared_ptr<CSMessage> msg) override;

    // server���Ե�accept��handler��������
    virtual void HandlerAcceptOverflow() override;
};

