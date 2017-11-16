#pragma once

#include <chrono>

namespace knet {

using DiffTime = std::chrono::system_clock::duration;

class KMessage;
class KMessageContext;

using MessagePtr = std::shared_ptr<KMessage>;
using MessageContextPtr = std::shared_ptr<KMessageContext>;


} // knet
