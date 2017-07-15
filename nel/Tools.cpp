#include "Tools.h"

#include "CMessage.h"

namespace nlnet {

namespace tools {

void protoToCMessage(const ::google::protobuf::Message& proto, CMessage* msg)
{
    std::vector<uint8_t> buffer{};
    buffer.resize(proto.ByteSize());
    proto.SerializeToArray(buffer.data(), static_cast<int>(buffer.size()));
    msg->parseFromArray(buffer);
}

bool messageToProto(const CMessage& msg, ::google::protobuf::Message* proto)
{
    return proto->ParseFromArray(msg.data(), static_cast<int>(msg.size()));
}

} // tools
} // NLNET
