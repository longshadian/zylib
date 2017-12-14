#include "net/Message.h"

namespace network {

void ServerCallback::HandlerAccept(Hdl)
{

}

void ServerCallback::HandlerClosed(Hdl)
{

}

void ServerCallback::HandlerTimeout(Hdl)
{

}

void ServerCallback::ReceviedMessage(Hdl hdl, std::shared_ptr<Message> msg)
{

}

void ServerCallback::HandlerAcceptOverflow()
{

}

} // network
