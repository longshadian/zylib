#pragma once

#include <array>
#include <vector>
#include <chrono>
#include <string>

struct NetworkMsg
{
    std::chrono::system_clock::time_point m_timestamp{};
    int32_t                               m_msg_id;
    std::string                           m_data{};
};
