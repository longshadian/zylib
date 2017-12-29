#include "SIDManager.h"

void SIDManager::ReplaceSID(SID_Set s)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_set = std::move(s);
}

bool SIDManager::IsValidSID(const std::string& sid) const
{
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_set.find(sid) != m_set.end();
}

