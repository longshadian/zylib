#pragma once

#include <vector>
#include <memory>

#include "Types.h"

namespace nlnet {

struct CMessage
{
    //using DATA = std::vector<uint8_t>;
    using DATA = std::string;

    CMessage(std::string name, std::string content)
        : m_msg_name(std::move(name))
        , m_data(std::move(content))
    {
    }

    CMessage() = default;

    std::string getMsgName() const
    {
        return m_msg_name;
    }

    const void* data() const
    {
        return m_data.data();
    }

    size_t size() const
    {
        return m_data.size();
    }

    const DATA& getData() const &
    {
        return m_data;
    }

    DATA& getData() &
    {
        return m_data;
    }

    DATA getData() &&
    {
        DATA temp = std::move(m_data);
        return temp;
    }

    // TODO …æ≥˝¡Ÿ ±≤‚ ‘¥˙¬Î
    std::vector<uint8_t> serializeToArray()const
    {
        std::vector<uint8_t> buffer{};
        buffer.resize(m_msg_name.size() + 1 + m_data.size());
        size_t pos = 0;
        for (auto c : m_msg_name) {
            buffer[pos] = static_cast<uint8_t>(c);
            ++pos;
        }
        // ≤π0
        buffer[pos] = 0;
        ++pos;

        for (auto c : m_data) {
            buffer[pos] = static_cast<uint8_t>(c);
            ++pos;
        }
        return buffer;
    }

    bool parseFromArray(const std::vector<uint8_t>& buffer)
    {
        size_t pos = 0;
        for (size_t i = 0; i != buffer.size(); ++i) {
            if (buffer[i] == 0) {
                pos = i;
                break;
            }
        }
        const char* p = (const char*)buffer.data();
        m_msg_name.assign(p, p + pos);

        for (size_t i = pos + 1; i != buffer.size(); ++i) {
            m_data.push_back((char)buffer[i]);
        }
        return true;
    }
    
    std::string          m_msg_name;
    DATA m_data;
};

struct NetworkMessage
{
    NetworkMessage() = default;

    TSockHdl m_sock_hdl;
    CMessage m_msg;
};

struct NetworkMessageContext
{
    NetworkMessageContext() = default;

    // TODO
    std::string  m_service_name;
    ServiceID    m_service_id;
    TSockPtr     m_sock;
};

} // nlnet
