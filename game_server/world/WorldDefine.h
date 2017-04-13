#ifndef _WORLD_DEFINE_H_
#define _WORLD_DEFINE_H_

#include "AppLog.h"

enum class SESSION_STATE 
{
    CLOSED                  = 0,    //关闭
    UNLOGIN                 = 1,    //未登录
    LOGIN                   = 2,    //已登录
    IGNORE                  = 3,    //无状态
    WAIT_REGISTER_FINISH    = 4,    //等待注册完成
    WAIT_LOGIN_FINISH       = 5,    //等待登录完成
};

#define PARSE_REQ(req, msg)                                                     \
do {                                                                            \
    if (!req.ParseFromArray(msg.data(), msg.size())) {            \
        appLog(LOG_ERROR, "parse error %s", req.GetTypeName().c_str());           \
        return;                                                                 \
    }                                                                           \
} while(0)


#endif
