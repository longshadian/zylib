
#include "network/Timer.h"

namespace network
{

void TimerCancel(TimerPtr timer)
{
    boost::system::error_code ec;
    timer->cancel(ec);
}

} // namespace network
