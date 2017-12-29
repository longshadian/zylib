#pragma once

#include <memory>
#include <chrono>
#include <array>
#include <type_traits>

#include "cs/CSMessage.h"

using ConnID = int64_t;

class RWHandler;
using RWHandlerPtr = std::shared_ptr<RWHandler>;
using Hdl = std::weak_ptr<RWHandler>;
using HdlLess = std::owner_less<Hdl>;

class Message;
using MessagePtr = std::shared_ptr<Message>;

