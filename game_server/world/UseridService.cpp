#include "UseridService.h"

#include "Log.h"

UseridService::UseridService()
    : m_max_userid(DEFAULT_MAX_USERID)
    , m_current_userid(0)
{
}

bool UseridService::init()
{
    LOG(DEBUG) << "userid service init...";
    return true;
}
