
#include "zylib/zylib.h"
#include "stream_server.h"

inline 
std::string getString(const network::CallbackMessage& msg)
{
    if (msg.m_data.empty())
        return {};
    std::string ret{msg.m_data.begin(), msg.m_data.end()};
    return ret;
}

inline
std::vector<uint8_t> getBinary(const std::string& str)
{
    if (str.empty())
        return {};
    std::vector<uint8_t> ret{ str.begin(), str.end() };
    return ret;
}

inline
network::CallbackMessagePtr createMessage(int32_t msg_id, const std::string& str)
{
    return network::callbackMessageEncoder(msg_id, getBinary(str));
}
