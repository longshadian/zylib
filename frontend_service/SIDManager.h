#pragma once

#include <string>
#include <mutex>
#include <unordered_set>

using SID_Set = std::unordered_set<std::string>;

class SIDManager
{
public:
    SIDManager() = default;
    ~SIDManager() = default;
    SIDManager(const SIDManager& rhs) = delete;
    SIDManager& operator=(const SIDManager& rhs) = delete;
    SIDManager(SIDManager&& rhs) = delete;
    SIDManager& operator=(SIDManager&& rhs) = delete;

    void ReplaceSID(SID_Set s);
    bool IsValidSID(const std::string& sid) const;

private:
    mutable std::mutex m_mtx;
    SID_Set    m_set;
};
