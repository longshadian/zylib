#include "FESCallback.h"

void FESCallback::HandlerAccept(Hdl)
{

}

void FESCallback::HandlerClosed(Hdl)
{

}

void FESCallback::HandlerTimeout(Hdl)
{

}

void FESCallback::ReceviedMessage(Hdl hdl, std::shared_ptr<Message> msg)
{
    (void)hdl;
    (void)msg;
}

void FESCallback::HandlerAcceptOverflow()
{

}
