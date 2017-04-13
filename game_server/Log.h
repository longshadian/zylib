#pragma once

#include "zylib/Logger.h"

inline
void initLog()
{
    zylib::logger::LogOptional opt{};
    opt.m_file_name_pattern = "./log/game_%Y-%m-%d.%3N.log";
    zylib::logger::init(opt);
}
