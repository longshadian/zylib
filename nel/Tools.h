#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>

#include <google/protobuf/message.h>

#include "Types.h"

namespace NLNET {

class CMessage;

namespace tools {

void protoToCMessage(const ::google::protobuf::Message& proto, CMessage* msg);
bool messageToProto(const CMessage& msg, ::google::protobuf::Message* proto); 

} // tools
} // NLNET
