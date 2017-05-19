#pragma once

#include "Types.h"

namespace NLNET {

class NetBase
{
public:
    NetBase();
    virtual ~NetBase();

    virtual void send(const CMessage& buffer, CUnifiedConnectionPtr conn) = 0;
    virtual uint64_t getReceiveQueueSize() = 0;
    virtual uint64_t getSendQueueSize() = 0;

    virtual void displayReceiveQueueStat() = 0;
    virtual void displaySendQueueStat() = 0;

    virtual bool flush(CUnifiedConnectionPtr conn) = 0;
    virtual void update(DiffTime diff_time) = 0;
    virtual bool connected() const = 0;
    virtual void disconnect(CUnifiedConnectionPtr conn) = 0;
};

}
