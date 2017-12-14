#pragma once

#include <cstdint>
#include <chrono>
#include <memory>

#include "net/NetworkType.h"

class Message
{
public:
    Message() = default;
    ~Message() = default;
    Message(const Message& rhs) = delete;
    Message& operator=(const Message& rhs) = delete;
    Message(Message&& rhs) = delete;
    Message& operator=(Message&& rhs) = delete;

    MsgHead                 m_head;
    std::vector<uint8_t>    m_body;
    std::chrono::system_clock::time_point m_timestamp;
};

class ServerCallback
{
public:
    ServerCallback() = default
    virtual ~ServerCallback() = default;
    ServerCallback(const ServerCallback& rhs) = delete;
    ServerCallback& operator=(const ServerCallback& rhs) = delete;
    ServerCallback(ServerCallback&& rhs) = delete;
    ServerCallback& operator=(ServerCallback&& rhs) = delete;

    // �µ�handler������
    virtual void HandlerAccept(Hdl hdl);

    // handler�ر�
    virtual void HandlerClosed(Hdl hdl);

    // handler��ʱ
    virtual void HandlerTimeout(Hdl hdl);

    // handler�յ���Ϣ
    virtual void ReceviedMessage(Hdl hdl, std::shared_ptr<Message> msg);

    // server���Ե�accept��handler��������
    virtual void HandlerAcceptOverflow();
};
