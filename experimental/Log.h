#ifndef _LOG_H_
#define _LOG_H_

#include <cstdarg>
#include <atomic>
#include <string>
#include <memory>
#include <map>
#include <thread>

#include "Queue.h"

enum LOG_TYPE
{
	LOG_DEBUG    =  1,
	LOG_TIP      =  2,
	LOG_TJ       =  4,
	LOG_ERROR    =  8,
    LOG_TRACE    = 16,
};

class ServerLoger
{
    class LogApp
    {
    public:
        LogApp(const char* log_root, const char* app_name);
        ~LogApp();
        void log(const char* log_type, const char* log_content);
    protected:
        FILE* 	m_file;
        char	m_app_name[256];
        int		m_day;
    };

    ServerLoger();
public:
    struct log_t
    {
        int  m_type;
        char m_app_name[64];
        char m_app_log[1024];
    };
public:
    ~ServerLoger();
    static ServerLoger& getInstance();

    void start();
    void setLogLevel(int level);
    int getLogLevel() const;
    void log(log_t& log);
private:
    void run();
    std::string getFullAppName();
    std::string getRootDir();
    static int getexepath(char* path, int max);
private:
    int                     m_log_level;
    std::atomic<bool>       m_running;
    std::thread             m_thread;
    std::string             m_log_root;
    ThreadSafeQueue<log_t>  m_log_queue;
    std::map<std::string, std::shared_ptr<LogApp>>  m_log_apps;
};

void PRINT_LOG(const char* app_name, int log_type, const char* log_content);
void PRINT_LOG_EX(const char* app_name, int log_type, const char* fmt, ...);

#endif
