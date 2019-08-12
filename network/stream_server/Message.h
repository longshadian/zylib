#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace network {

class Message
{
public:
    Message() {}
    virtual ~Message() {}

    Message(const Message& rhs) = delete;
    Message& operator=(const Message& rhs) = delete;
    Message(Message&& rhs) = delete;
    Message& operator=(Message&& rhs) = delete;

    virtual const void* data() const = 0;
    virtual size_t size() const = 0;

};

}
