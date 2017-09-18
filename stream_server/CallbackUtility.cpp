#include "CallbackUtility.h"

#include <cstring>
#include <boost/format.hpp>

#include "ByteBuffer.h"
#include "CallbackMessage.h"

namespace network {

void callbackMessageDecoder(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    (void)hdl;
    while (true) {
        auto buffer_size = static_cast<int32_t>(buffer.byteSize());
        if (buffer_size < 8)
            break;

        int32_t len = 0;
        buffer.read(&len);
        if (len < 8) {
            std::string str = boost::str(boost::format("read head len error. %1%") % len);
            throw std::runtime_error(std::move(str));
        }
        if (len <= buffer_size) {
            buffer.readSkip(sizeof(len));

            int32_t msg_id = 0;
            buffer >> msg_id;
            auto msg = std::make_shared<CallbackMessage>();
            msg->m_msg_id = msg_id;
            if (len - 8 > 0) {
                buffer >> msg->m_data;
            }
            out->push_back(msg);
        } else {
            break;
        }
    }
}

CallbackMessagePtr callbackMessageEncoder(int32_t msg_id, const std::vector<uint8_t>& data)
{
    auto msg = std::make_shared<CallbackMessage>();
    msg->m_msg_id = msg_id;
    msg->m_data.resize(8 + data.size());
    auto pos = msg->m_data.data();
    int32_t len = static_cast<int32_t>(msg->m_data.size());
    std::memcpy(pos, &len, 4);
    pos += 4;

    std::memcpy(pos, &msg_id, 4);
    pos += 4;
    if (!data.empty())
        std::memcpy(pos, data.data(), data.size());
    return msg;
}

} // network
