#include "stdafx.h"

#include <sstream>
#include <string>
#include "Common.h"

bool Common::EncodeMessage(std::int32_t cmd, const boost::property_tree::ptree& pt, std::string* out)
{
    try {
        boost::property_tree::ptree pt_root;
        pt_root.put<std::int32_t>("cmd", cmd);
        pt_root.put_child("data", pt);

        std::ostringstream ostm{};
        boost::property_tree::write_json(ostm, pt_root);
        *out = ostm.str();
        return true;
    } catch (const std::exception& e) {
        (void)e;
        return false;
    }
}

bool Common::EncodeMessage(std::int32_t cmd, const std::string& data, std::string* out)
{
    try {
        boost::property_tree::ptree pt_root;
        pt_root.put<std::int32_t>("cmd", cmd);
        pt_root.put<std::string>("datastr", data);
        std::ostringstream ostm{};
        boost::property_tree::write_json(ostm, pt_root);
        *out = ostm.str();
        return true;
    } catch (const std::exception& e) {
        (void)e;
        return false;
    }
}

