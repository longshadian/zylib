#include "ServerApp.h"

#include <unistd.h>
#include <csignal>
#include <cstdarg>

#include <iosfwd>
#include <fstream>
#include <thread>
#include <chrono>

#include "zylib/Random.h"

#include "Log.h"
#include "Config.h"
#include "GlobalService.h"

void serverAbort()
{
    std::abort();
}

void sigTerm(int signo)
{
    if (signo == SIGTERM) {
        ServerApp::instance().stop();
    }
}

ServerApp::ServerApp()
    : m_is_running(false)
{
}

ServerApp::~ServerApp()
{
}

ServerApp& ServerApp::instance()
{
    static ServerApp _instance;
    return _instance;
}

bool ServerApp::start()
{
    ::signal(SIGTERM, &::sigTerm);
    ::signal(SIGHUP, SIG_IGN);
    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGALRM, SIG_IGN);
    ::signal(SIGCHLD, SIG_IGN);

    //守护进程
    ::daemon(1, 0);

    //内存不足时奔溃
    std::set_new_handler(&::serverAbort);

    //初始化随机数引擎
    zylib::randomEngineInit();

    m_start_time = std::chrono::system_clock::now();

    initLog();

    //警告:以下顺序变更时请小心
    //1.先获取服务器配置
    if (!config::instance().init()) {
        LOG(ERROR) << "ServerApp initConf";
        return false;
    }

    if (!Global::init()) {
        LOG(ERROR) << "global init fail";
        return false;
    }

    m_is_running = true;
	LOG(INFO) << "every thing init ok";
    return true;
}

void ServerApp::loop()
{
    while (m_is_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOG(INFO) << "ServerApp exit successed";
}

void ServerApp::stop()
{
    m_is_running.exchange(false);
}


const std::chrono::system_clock::time_point& ServerApp::getStartTime() const
{
    return m_start_time;
}

ServerRunTime ServerApp::getRunTime() const
{
    ServerRunTime run_time{};
    auto tnow = std::chrono::system_clock::now();
    if (tnow <= m_start_time)
        return run_time;
    run_time.m_total_minute = std::chrono::duration_cast<std::chrono::minutes>(tnow - m_start_time).count();
    auto remain = run_time.m_total_minute;
    run_time.m_day = remain/(24*60);
    remain = remain%(24*60);
    run_time.m_hour = remain/60;
    run_time.m_minute = remain%60;
    return run_time;
}
