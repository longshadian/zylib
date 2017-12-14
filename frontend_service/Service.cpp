#include "Service.h"

#include <unistd.h>
#include <csignal>
#include <cstdarg>

#include <iosfwd>
#include <fstream>
#include <thread>
#include <chrono>

#include "FakeLog.h"
#include "world/World.h"

void serverAbort()
{
    std::abort();
}

void sigTerm(int signo)
{
    if (signo == SIGTERM) {
        Service::instance().stop();
    }
}

Service::Service()
    : m_is_running(false)
{
}

Service::~Service()
{
}

Service& Service::instance()
{
    static Service _instance;
    return _instance;
}

bool Service::start()
{
    ::signal(SIGTERM, &::sigTerm);
    ::signal(SIGHUP, SIG_IGN);
    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGALRM, SIG_IGN);
    ::signal(SIGCHLD, SIG_IGN);

    //守护进程
    ::daemon(1, 0);

    fake_log::initLog(std::make_unique<fake_log::ConsoleLog>());

    //内存不足时奔溃
    std::set_new_handler(&::serverAbort);

    std::this_thread::sleep_for(std::chrono::seconds{1});

    m_is_running = true;
	FAKE_LOG(INFO) << "every thing init ok";
    return true;
}

void Service::loop()
{
    while (m_is_running) {
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
	FAKE_LOG(INFO) << "ServerApp exit successed";
}

void Service::stop()
{
    m_is_running.exchange(false);
}

Service& GetService()
{
    return Service::instance();
}

World& Service::GetWorld()
{
    return *m_world;
}

World& GetWorld()
{
    return ::GetService().GetWorld();
}
