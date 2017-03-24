#include "Log.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

ServerLoger::LogApp::LogApp(const char* log_root, const char* app_name)
{
    m_file = NULL;
    memset(m_app_name, 0, sizeof(m_app_name));
    m_day = 0;

    time_t curTime = time(NULL);
    struct tm cur_tm;
    localtime_r(&curTime, &cur_tm);
    m_day = cur_tm.tm_mday;

    std::string app_log_dir = log_root;
    app_log_dir += "/";
    app_log_dir += app_name;
    strncpy(m_app_name, app_log_dir.c_str(), sizeof(m_app_name));
    mkdir(m_app_name, S_IWUSR | S_IXUSR | S_IRUSR);
}

ServerLoger::LogApp::~LogApp()
{
    if (m_file)
        fclose(m_file);
}

void ServerLoger::LogApp::log(const char* log_type, const char* log_content)
{
    time_t t = time(NULL);
    struct tm cur_tm;
    localtime_r(&t, &cur_tm);
    if (cur_tm.tm_mday != m_day) {
        m_day = cur_tm.tm_mday;
        if (m_file)
            fclose(m_file);
        m_file = NULL;
    }

    char log_head[64];
    snprintf(log_head, sizeof(log_head), "[%s] %02d:%02d:%02d ",
        log_type, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec);
    if (!m_file) {
        char file_name[256];
        snprintf(file_name, sizeof(file_name), "%s/%04d%02d%02d.log",
            m_app_name, cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday);
        m_file = fopen(file_name, "a+");
    }
    if (m_file) {
        fwrite(log_head, 1, strlen(log_head), m_file);
        fwrite(log_content, 1, strlen(log_content), m_file);
        char c = '\n';
        fwrite(&c, 1, 1, m_file);
        fflush(m_file);
    }
}

//////////////////////////////////////////////////////////////////////////
ServerLoger::ServerLoger() 
	: m_log_level(LOG_DEBUG | LOG_TIP | LOG_TJ | LOG_ERROR | LOG_TRACE)
    , m_running(false)
    , m_thread()
    , m_log_root()
    , m_log_queue()
    , m_log_apps()
{
    
    //¼ÙÉèÂ·¾¶ /home/xxx/server_install_path/bin/server
    //                                     /log

    //m_log_root = getRootDir();

    std::string full_app_name = getFullAppName();

    if (full_app_name.length() > 0) {
        int count = 0;
        auto it = full_app_name.rbegin();
        for (; it != full_app_name.rend(); ++it) {
            if (*it == '/')
                count++;

            //È¥³ý bin/server
            if (count == 2) {
                break;
            }
        }
        std::string temp(it, full_app_name.rend());
        m_log_root.assign(temp.rbegin(), temp.rend());
        m_log_root += "log";
    }

    if (!m_log_root.empty()) {
        mkdir(m_log_root.c_str(), S_IWUSR | S_IXUSR | S_IRUSR);
    }
}

ServerLoger::~ServerLoger()
{
    m_running = false;
    m_log_queue.push(log_t());
    if (m_thread.joinable())
        m_thread.join();
}

ServerLoger& ServerLoger::getInstance()
{
    static ServerLoger _instance;
    return _instance;
}

void ServerLoger::log(log_t& msg)
{
    m_log_queue.push(msg);
}

void ServerLoger::setLogLevel(int level)
{
    m_log_level = level;
}

int ServerLoger::getLogLevel() const
{
    return m_log_level;
}

void ServerLoger::run()
{
    log_t msg;
    while(m_running) {
        memset(&msg, 0, sizeof(msg));
        m_log_queue.waitAndPop(msg);

        std::shared_ptr<LogApp> app;
        auto it = m_log_apps.find(msg.m_app_name);
        if (it == m_log_apps.end()) {
            if (strlen(msg.m_app_name) > 0) {
                app = std::make_shared<LogApp>(m_log_root.c_str(), msg.m_app_name);
                m_log_apps[msg.m_app_name] = app;
            }
        } else {
            app = it->second;
        }

        if(app) {
            char log_type[32];
            switch(msg.m_type) {
            case LOG_TRACE:
                strcpy(log_type, "TRACE");
                break;
            case LOG_DEBUG:
                strcpy(log_type, "DEBUG");
                break;
            case LOG_TJ:
                strcpy(log_type, "TJ   ");
                break;
            case LOG_TIP:
                strcpy(log_type, "TIP  ");
                break;
            default:
                strcpy(log_type, "ERROR");
                break;
            }
            app->log(log_type, msg.m_app_log);
        }
    }
}

std::string ServerLoger::getFullAppName()
{
    char full_app_name[256] = { 0 };
    getexepath(full_app_name, sizeof(full_app_name));
    if (strlen(full_app_name) > 0)
        return full_app_name;
    return std::string();
}

std::string ServerLoger::getRootDir()
{
    std::string full_path;
    char full_app_name[256] = { 0 };
    getexepath(full_app_name, sizeof(full_app_name));

    if (strlen(full_app_name) > 0) {
        char* name = strrchr(full_app_name, '/');
        if (name) {
            *name = 0;
            name = strrchr(full_app_name, '/');
            if (name) {
                name++;
                *name = 0;
                full_path = full_app_name;
            }
        }
    }
    return full_path + "log";
}

int ServerLoger::getexepath(char* path, int max)
{
    char buf[128];
    sprintf(buf, "/proc/%d/exe", getpid());
    int n = static_cast<int>(readlink(buf, path, max));
    return n;
}

void ServerLoger::start()
{
    m_running = true;
    std::thread temp(std::bind(&ServerLoger::run, this));
    m_thread = std::move(temp);
}

void PRINT_LOG(const char* app_name, int log_type, const char* log_content)
{
	if((ServerLoger::getInstance().getLogLevel() & log_type) == 0)
		return;

    ServerLoger::log_t one_log;
    memset(&one_log, 0, sizeof(one_log));
    strncpy(one_log.m_app_name, app_name, sizeof(one_log.m_app_name)-1);
	one_log.m_type = log_type;
    strncpy(one_log.m_app_log, log_content, strlen(log_content));
    ServerLoger::getInstance().log(one_log);
}

void PRINT_LOG_EX(const char* app_name, int log_type, const char* format, ...)
{
	if((ServerLoger::getInstance().getLogLevel() & log_type) == 0)
		return;

    ServerLoger::log_t one_log;
    memset(&one_log, 0, sizeof(one_log));
    va_list va;
    va_start(va, format);
    vsnprintf(one_log.m_app_log, sizeof(one_log.m_app_log)-1, format, va);
    va_end(va);
    strncpy(one_log.m_app_name, app_name, sizeof(one_log.m_app_name)-1);
	one_log.m_type = log_type;
    ServerLoger::getInstance().log(one_log);
}
