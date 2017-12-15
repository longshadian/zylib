#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <vector>
#include <string>

#include "net/NetworkType.h"

class SendMessage
{
public:
    SendMessage() = default;
    ~SendMessage() = default;
    SendMessage(const SendMessage& rhs) = delete;
    SendMessage& operator=(const SendMessage& rhs) = delete;
    SendMessage(SendMessage&& rhs) = delete;
    SendMessage& operator=(SendMessage&& rhs) = delete;

    const uint8_t* data() const { return m_buffer.data(); }
    size_t size() const { return m_buffer.size(); }

    std::vector<uint8_t> m_buffer;
};

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

    int32_t     GetMsgID() const;
    std::string GetSID() const;
};

class ServerCallback
{
public:
    ServerCallback() = default;
    virtual ~ServerCallback() = default;
    ServerCallback(const ServerCallback& rhs) = delete;
    ServerCallback& operator=(const ServerCallback& rhs) = delete;
    ServerCallback(ServerCallback&& rhs) = delete;
    ServerCallback& operator=(ServerCallback&& rhs) = delete;

    // 新的handler创建了
    virtual void HandlerAccept(Hdl hdl);

    // handler关闭
    virtual void HandlerClosed(Hdl hdl);

    // handler超时
    virtual void HandlerTimeout(Hdl hdl);

    // handler收到消息
    virtual void ReceviedMessage(Hdl hdl, std::shared_ptr<Message> msg);

    // server可以得accept的handler超出上限
    virtual void HandlerAcceptOverflow();
};
