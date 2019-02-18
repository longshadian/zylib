#ifndef _SERVER_APP_H_
#define _SERVER_APP_H_

#include <atomic>
#include <string>
#include <chrono>

struct ServerRunTime
{
    int64_t m_day{0};
    int64_t m_hour{0};
    int64_t m_minute{0};
    int64_t m_total_minute{0};
};

class ServerApp
{
public:
    ServerApp();
    ~ServerApp();
    static ServerApp& instance();

    bool start();
    void loop();
    void stop();

    const std::chrono::system_clock::time_point& getStartTime() const;
    ServerRunTime getRunTime() const;
private:
	bool initGameData();

private:
    std::atomic<bool>               m_is_running;
    std::chrono::system_clock::time_point m_start_time;
};

#endif
