#include "Log.h"

#include "zylib/Tools.h"

void initLog()
{
    zylib::logger::LogOptional opt{};
    opt.m_file_name_pattern = "./log/login_%Y-%m-%d.%3N.log";
    zylib::logger::init(opt);

    auto network_log = zylib::make_unique<NetworkLog>();
    network::initLog(std::move(network_log));
}

NetworkLog::NetworkLog()
{
}

NetworkLog::~NetworkLog()
{
}

void NetworkLog::flush()
{
    std::string s{};
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        s = m_ostm.str();
        m_ostm.str("");
    }
    LOG(DEBUG) << s;
}
