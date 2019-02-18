#include "NSTools.h"

#include <cstring>

namespace nlnet {
namespace ns {

NSMessagePtr decode(std::vector<uint8_t> src)
{
    int32_t pos = -1;
    for (size_t i = 0; i != src.size(); ++i) {
        if (src[i] == 0) {
            pos = static_cast<int32_t>(i);
            break;
        }
    }
    if (pos == -1 || pos == 0) {
        return nullptr;
    }

    std::string msg_name(src.begin(), src.begin() + pos - 1);
    const google::protobuf::Descriptor* descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(msg_name);
    if (!descriptor)
        return nullptr;
    const google::protobuf::Message* prototype =
        google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (!prototype)
        return nullptr;
    google::protobuf::Message* p_msg = prototype->New();
    if (!p_msg->ParseFromArray(src.data() + pos + 1, static_cast<int>(src.size()) - pos - 1))
        return nullptr;
    return std::make_shared<NSMessage>(p_msg, std::move(msg_name));
}

std::vector<uint8_t> encode(const google::protobuf::Message& msg)
{
    auto msg_name = msg.GetTypeName();
    std::vector<uint8_t> buffer{};
    buffer.resize(msg_name.size() + 1 + msg.ByteSize());
    auto* pos = buffer.data();
    std::memcpy(pos, msg_name.c_str(), msg_name.size());
    pos += msg_name.size() + 1;
    if (!msg.SerializePartialToArray(pos, msg.ByteSize()))
        return {};
    return buffer;
}

} // ns

} // nlnet
