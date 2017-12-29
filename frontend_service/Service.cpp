#include "Service.h"

#include <unistd.h>
#include <csignal>
#include <cstdarg>

#include <iosfwd>
#include <fstream>
#include <thread>
#include <chrono>

#include "SIDManager.h"
#include "FakeLog.h"
#include "world/World.h"
#include "net/StreamServer.h"

void OutOfMemory()
{
    std::abort();
}

void sigTerm(int signo)
{
    if (signo == SIGTERM) {
        Service::instance().Stop();
    }
}

Service::Service()
    : m_is_running(false)
    , m_world()
    , m_sid_mgr(std::make_unique<SIDManager>())
    , m_knet(std::make_unique<::knet::UniformNetwork>())
    , m_network()
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

bool Service::Start()
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
    std::set_new_handler(&::OutOfMemory);

    std::this_thread::sleep_for(std::chrono::seconds{1});
    m_world = std::make_unique<World>();
    if (m_world->Init()) {
        FAKE_LOG(WARNING) << "world init fail.";
        return false;
    }

    // TODO knet

    m_network = std::make_unique<StreamServer>(8087);

    m_is_running = true;
	FAKE_LOG(INFO) << "every thing init ok";
    return true;
}

void Service::Loop()
{
    while (m_is_running) {
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
	FAKE_LOG(INFO) << "ServerApp exit successed";
}

void Service::Stop()
{
    m_is_running.exchange(false);
    m_world->Stop();
    m_world->WaitTheadExit();
}

SIDManager& Service::GetSIDManager()
{
    return *m_sid_mgr;
}

Service& GetService()
{
    return Service::instance();
}

SIDManager& GetSIDManager()
{
    return ::GetService().GetSIDManager();
}

World& Service::GetWorld()
{
    return *m_world;
}

::knet::UniformNetwork& Service::GetKNet()
{
    return *m_knet;
}

World& GetWorld()
{
    return ::GetService().GetWorld();
}

::knet::UniformNetwork& GetKNet()
{
    return ::GetService().GetKNet();
}
