#pragma once

#include <string>
#include <mutex>
#include <unordered_set>


// 必须是先线程安全的
class SIDManager
{
public:
    SIDManager() = default;
    ~SIDManager() = default;
    SIDManager(const SIDManager& rhs) = delete;
    SIDManager& operator=(const SIDManager& rhs) = delete;
    SIDManager(SIDManager&& rhs) = delete;
    SIDManager& operator=(SIDManager&& rhs) = delete;

    void ReplaceSID(std::unordered_set<std::string> s);
    bool IsValidSID(const std::string& sid) const;
private:
    mutable std::mutex m_mtx;
    std::unordered_set<std::string>  m_set;
};
