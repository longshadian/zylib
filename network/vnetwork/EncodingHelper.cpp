#include "stdafx.h"

#include "vnetwork/EncodingHelper.h"
#include <sstream>
#include "Win32Transcode.h"


std::string EncodingHelper::OpenUrlJson(const std::string& url)
{
    std::string msg = "打开客户端";
    std::string msg_utf8;
    Win32Transcode::ANSI_to_UTF8(msg.data(), msg.length(), msg_utf8);

    std::ostringstream ostm{};
    const char* fmt = R"({"type":2001, "data":{"msg":"%s", "url": "%s"}})";

    char buffer[1024] = { 0 };
    sprintf_s(buffer, sizeof(buffer), fmt, msg_utf8.c_str(), url.c_str());

    std::string s = buffer;
    return s;
}

