#include "FESCallback.h"

#include "Service.h"
#include "world/world.h"

void FESCallback::HandlerAccept(Hdl hdl)
{
    ::GetWorld().NetworkAccept(std::move(hdl));
}

void FESCallback::HandlerClosed(Hdl hdl)
{
    ::GetWorld().NetworkClosed(std::move(hdl));
}

void FESCallback::HandlerTimeout(Hdl hdl)
{
    ::GetWorld().NetworkTimeout(std::move(hdl));
}

void FESCallback::ReceviedMessage(Hdl hdl, std::shared_ptr<CSMessage> msg)
{
    ::GetWorld().NetworkReceviedMsg(std::move(hdl), std::move(msg));
}

void FESCallback::HandlerAcceptOverflow()
{

}
