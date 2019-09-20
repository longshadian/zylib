#include "network/MessageDecoder.h"

#include <cstring>
#include <array>
#include "network/Define.h"
#include "network/Utilities.h"
#include "network/Message.h"

namespace network
{

class DefaultMessageDecoder : public MessageDecoder
{
public:
    DefaultMessageDecoder() = default;
    virtual ~DefaultMessageDecoder() = default;

    virtual void Encode(const void* data, std::size_t length, Message* msg) override;
    virtual void Decode(FlatBuffer& buffer, std::vector<Message>* vec) override;

private:
    bool DecodeOnce(FlatBuffer& buffer, Message* out);
};

void DefaultMessageDecoder::Encode(const void* data, std::size_t length, Message* out)
{
    MessageHead head{};
    Utilities::BZero(&head);
    head.m_body_length = static_cast<std::int32_t>(length);

    std::size_t msg_length = HEAD_LENGTH + length;
    out->m_buffer.Reverse(msg_length);
    out->m_buffer.Append(&head, HEAD_LENGTH);
    out->m_buffer.Append(data, length);
}

void DefaultMessageDecoder::Decode(FlatBuffer& buffer, std::vector<Message>* vec)
{
    while (1) {
        Message msg{};
        if (!DecodeOnce(buffer, &msg)) {
            break;
        }
        vec->emplace_back(std::move(msg));
    }
}

bool DefaultMessageDecoder::DecodeOnce(FlatBuffer& buffer, Message* msg)
{
    if (buffer.Length() < HEAD_LENGTH) {
        return false;
    }
    MessageHead head{};
    Utilities::BZero(&head);
    std::memcpy(&head, buffer.Ptr(), HEAD_LENGTH);
    std::size_t msg_length = HEAD_LENGTH + head.m_body_length;
    if (buffer.Length() < msg_length) {
        return false;
    }
    msg->m_buffer.Append(buffer.Ptr(), msg_length);
    NETWORK_ASSERT(buffer.Consume(msg_length));
    return true;
}

MessageDecoderPtr CreateDefaultMessageDecoder()
{
    static auto p = std::make_shared<DefaultMessageDecoder>();
    return p;
}

} // namespace network
