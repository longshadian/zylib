#ifndef _USERID_SERVICE_H_
#define _USERID_SERVICE_H_

#include <cstdint>
#include <string>

class UseridService
{
    static const uint64_t DEFAULT_USERID_BEGIN  =      120000;
    static const uint64_t DEFAULT_MAX_USERID    = 1000000000;   
public:
    UseridService();
    ~UseridService() = default;

    bool        init();
    uint64_t    newUserid();
	uint64_t    getCurrUserid() const;
private:
    bool initCurrentGuid();
    static bool maxGuid(int32_t tableid, uint64_t* guid);
private:
    uint64_t   m_max_userid;
    uint64_t   m_current_userid;
};

#endif