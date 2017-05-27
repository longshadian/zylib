#pragma once

#include "Types.h"

namespace NLNET {

class NetBase : public std::enable_shared_from_this<NetBase>
{
public:
    NetBase();
    virtual ~NetBase();

    virtual void send(CMessage buffer, TSockPtr sock) = 0;

    virtual bool flush(CUnifiedConnectionPtr conn) = 0;
    virtual void update(DiffTime diff_time) = 0;
    virtual bool connected() const = 0;
    virtual void disconnect(CUnifiedConnectionPtr conn) = 0;
};

}
