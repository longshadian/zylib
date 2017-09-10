#include "Message.h"

#include <cstring>
#include <algorithm>

namespace network {

Message::Message(const std::string& data)
    :Message(data.c_str(), data.length())
{
}

Message::Message(const void* data, size_t len)
    : m_data()
{
    m_data.resize(len);
    std::memcpy(m_data.data(), data, len);
}

Message::Message(StorageType data)
    : m_data(std::move(data))
{
}

Message::~Message()
{

}

const void* Message::data() const
{
    if (m_data.empty())
        return nullptr;
    return m_data.data();
}

size_t Message::size() const
{
    return m_data.size();
}

}

