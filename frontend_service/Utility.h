#pragma once

#include <atomic>
#include <string>
#include <chrono>
#include <memory>

#include "net/NetworkMsg.h"

class Utility
{
public:
    bool EncryptMessage(std::shared_ptr<Message> msg);
    bool DecryptMessage();
};

