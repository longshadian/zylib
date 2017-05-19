#include "NetServer.h"

namespace NLNET {

NetServer::NetServer()
{
}

NetServer::~NetServer()
{
}

void NetServer::send(const CMessage& buffer, CUnifiedConnectionPtr conn)
{

}

uint64_t NetServer::getReceiveQueueSize()
{

}

uint64_t NetServer::getSendQueueSize()
{

}

void NetServer::displayReceiveQueueStat()
{

}

void NetServer::displaySendQueueStat()
{

}

bool NetServer::flush(CUnifiedConnectionPtr conn)
{

}

void NetServer::update(DiffTime diff_time)
{

}

bool NetServer::connected() const
{

}

void NetServer::disconnect(CUnifiedConnectionPtr conn)
{

}

}
