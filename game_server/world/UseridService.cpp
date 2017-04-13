#include "UseridService.h"

#include "AppLog.h"
#include "GameDefine.h"
#include "DBPool.h"
#include "DBOperator.h"

UseridService::UseridService()
    : m_max_userid(DEFAULT_MAX_USERID)
    , m_current_userid(0)
{
}

bool UseridService::init()
{
    appLog(LOG_TIP, "start...");

    uint64_t tguid_value = 0;
    uint64_t tguid_max_value = 0;

    //��ȡtguid�������ֵ
    DBOperator db_operator{ g_dbpool.getDBConnect(gamedef::DATABASE_ID_GAME) };
    if (!db_operator.selectGUID(gamedef::GUID_USERID, &tguid_value, &tguid_max_value)) {
        appLog(LOG_ERROR, "loadTable");
        return false;
    }

    //��ǰtuser���е����ֵ
    uint64_t tuser_value = 0;
    if (!db_operator.selectMaxUserid(&tuser_value)) {
        appLog(LOG_ERROR, "initCurrentGuid");
        return false;
    }

    //tuser��useridС��tguid����tguid���ֵ��
    m_max_userid = tguid_max_value;
    if (tuser_value < tguid_value) {
        tuser_value = tguid_value;
    }
    m_current_userid = tuser_value + 1;
    appLog(LOG_TIP, "guid current:%" PRIu64" max:%" PRIu64" end", m_current_userid, m_max_userid);
    return true;
}

uint64_t UseridService::newUserid()
{
    if (m_current_userid >= m_max_userid) {
        appLog(LOG_ERROR, "userid overflow current:%" PRIu64" max:%" PRIu64, m_current_userid, m_max_userid);
        return 0;
    }
    return uint64_t(m_current_userid++);
}

uint64_t UseridService::getCurrUserid() const
{
	return m_current_userid;
}