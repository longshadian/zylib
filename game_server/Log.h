#pragma once

#include <sstream>
#include <mutex>

#include "zylib/Logger.h"
#include "network/FakeLog.h"

void initLog();

class NetworkLog : public network::LogStream
{
public:
    NetworkLog();
    virtual ~NetworkLog();

    virtual void flush() override;
};
