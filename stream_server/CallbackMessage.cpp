#include "CallbackMessage.h"

namespace network {

CallbackMessage::CallbackMessage()
    : Message()
    , m_msg_id()
    , m_data()
{
}

CallbackMessage::~CallbackMessage()
{
}

const void* CallbackMessage::data() const
{
    if (m_data.empty())
        return nullptr;
    return m_data.data();
}

size_t CallbackMessage::size() const
{
    return m_data.size();
}

} // network
