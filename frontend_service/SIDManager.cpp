#include "SIDManager.h"

void SIDManager::ReplaceSID(std::unordered_set<std::string> s)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_set = std::move(s);
}

bool SIDManager::IsValidSID(const std::string& sid) const
{
    std::lock_guard<std::mutex> lk{m_mtx};
    return m_set.find(sid) != m_set.end();
}

