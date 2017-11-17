#pragma once

#include <chrono>

namespace knet {

using DiffTime = std::chrono::milliseconds;

class KMessage;
class KMessageContext;

using MessagePtr = std::shared_ptr<KMessage>;
using MessageContextPtr = std::shared_ptr<KMessageContext>;

using ServiceID = std::string;
using RPCKey = uint64_t;

} // knet
