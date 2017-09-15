#include <iostream>
#include <thread>
#include <atomic>
#include <memory>

#include "stream_server/StreamServer.h"
#include "stream_server/NetworkType.h"

#include "CallbackServer.h"

int main()
{
    network::CallbackServer s{"127.0.0.1", 32222};
    (void)s;

    return 0;
}