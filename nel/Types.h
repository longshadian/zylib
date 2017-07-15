#pragma once

#include <vector>
#include <memory>
#include <chrono>

namespace nlnet {

using ServiceID = int32_t;
using DiffTime = std::chrono::seconds;

class TSock;
using TSockPtr = std::shared_ptr<TSock>;
using TSockHdl = std::weak_ptr<TSock>;

struct CMessage;
using CMessagePtr = std::shared_ptr<CMessage>;

struct NetworkMessage;
using NetworkMessagePtr = std::shared_ptr<NetworkMessage>;

class UnifiedConnection;
using UnifiedConnectionPtr = std::shared_ptr<UnifiedConnection>;

class NetBase;
using NetBasePtr = std::shared_ptr<NetBase>;

class NetServer;
using NetServerPtr = std::shared_ptr<NetServer>;

class NetClient;
using NetClientPtr = std::shared_ptr<NetClient>;

} // nelnet
