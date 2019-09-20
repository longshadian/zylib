#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <vector>
#include <string>

namespace network
{

class FlatBuffer;
class Message;

class MessageDecoder
{
public:
    MessageDecoder() = default;
    virtual ~MessageDecoder() = default;
    MessageDecoder(const MessageDecoder& rhs) = delete;
    MessageDecoder& operator=(const MessageDecoder& rhs) = delete;
    MessageDecoder(MessageDecoder&& rhs) = delete;
    MessageDecoder& operator=(MessageDecoder&& rhs) = delete;

    virtual void Encode(const void* data, std::size_t length, Message* msg) = 0;
    virtual void Decode(FlatBuffer& buffer, std::vector<Message>* vec) = 0;
};

using MessageDecoderPtr = std::shared_ptr<MessageDecoder>;

MessageDecoderPtr CreateDefaultMessageDecoder();

} // namespace network
