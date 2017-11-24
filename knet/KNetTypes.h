#pragma once

#include <chrono>
#include <functional>

namespace knet {

using DiffTime = std::chrono::milliseconds;

class KMessage;
class KMessageContext;

using MessagePtr = std::shared_ptr<KMessage>;
using MessageContextPtr = std::shared_ptr<KMessageContext>;

using ServiceID = std::string;
using RPCKey = uint64_t;

using Duration = std::chrono::milliseconds;

using Callback = std::function<void()>;

} // knet
