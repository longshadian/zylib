#pragma once

#include <unordered_map>
#include <functional>

#include "net/NetworkType.h"
#include "world/Session.h"

using ClientCB = std::function<void(MessagePtr)>;
using ClientCB_Array = std::unordered_map<int32_t, ClientCB>;
