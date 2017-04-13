#ifndef _WORLD_DEFINE_H_
#define _WORLD_DEFINE_H_

#include "AppLog.h"

enum class SESSION_STATE 
{
    CLOSED                  = 0,    //�ر�
    UNLOGIN                 = 1,    //δ��¼
    LOGIN                   = 2,    //�ѵ�¼
    IGNORE                  = 3,    //��״̬
    WAIT_REGISTER_FINISH    = 4,    //�ȴ�ע�����
    WAIT_LOGIN_FINISH       = 5,    //�ȴ���¼���
};

#define PARSE_REQ(req, msg)                                                     \
do {                                                                            \
    if (!req.ParseFromArray(msg.data(), msg.size())) {            \
        appLog(LOG_ERROR, "parse error %s", req.GetTypeName().c_str());           \
        return;                                                                 \
    }                                                                           \
} while(0)


#endif
