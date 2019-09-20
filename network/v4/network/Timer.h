#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace network
{

using TimerPtr = std::shared_ptr<boost::asio::steady_timer>;

void TimerCancel(TimerPtr timer);

} // namespace network

