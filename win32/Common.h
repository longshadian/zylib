#pragma once

#include <cstdint>
#include <string>

#include <boost/property_tree/json_parser.hpp>

enum CMD 
{
    CMD_RegisterEvent                   = 100,
    CMD_OpenUserEvent                   = 101,
    CMD_CloseUserEvent                  = 102,
    CMD_AddToMemberEvent                = 103,
    CMD_PreOpenUserEvent                = 104,
};

class Common
{
public:
    static bool EncodeMessage(std::int32_t cmd, const boost::property_tree::ptree& pt, std::string* out);
    static bool EncodeMessage(std::int32_t cmd, const std::string& data, std::string* out);
};

