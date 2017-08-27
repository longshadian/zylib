#pragma once

#include <vector>
#include <string>
#include <memory>

#include <google/protobuf/message.h>

namespace nlnet {
namespace ns {

class NSMessage
{
public:
    NSMessage(google::protobuf::Message* msg, std::string msg_name)
        : m_data(msg)
        , m_msg_name(std::move(msg_name))
    {
    }

    ~NSMessage()
    {
        if (m_data)
            delete m_data;
    }

    NSMessage(const NSMessage&) = delete;
    NSMessage& operator=(const NSMessage&) = delete;
    NSMessage(NSMessage&&) = delete;
    NSMessage& operator=(NSMessage&&) = delete;

    const std::string& getMsgName() const
    {
        return m_msg_name;
    }

    const google::protobuf::Message* getMsg() const
    {
        return m_data;
    }

private:
    google::protobuf::Message*  m_data;
    std::string                 m_msg_name;
};

using NSMessagePtr = std::shared_ptr<NSMessage>;

NSMessagePtr decode(std::vector<uint8_t> src);

std::vector<uint8_t> encode(const google::protobuf::Message& msg);

} // ns

} // NLNET
