#pragma once

#include <ctime>
#include <sys/time.h>

#include <cmath>
#include <type_traits>

namespace zylib {

template <uint64_t TM_BITS, uint64_t WORKER_ID_BITS, uint64_t SEQUENCE_BITS>
class Snowflake
{
public:
    Snowflake(uint32_t worker_id, uint64_t start_epoch)
        : m_epoch(start_epoch)
        , m_worker_id(worker_id)
        , m_last_timestamp()
        , m_sequence()
    {
    }

    const uint64_t  m_epoch;
    const uint64_t  m_worker_id;

    uint64_t        m_last_timestamp;
    uint64_t        m_sequence;

    enum { MAX_TM        = uint64_t(1) << TM_BITS};
    enum { MAX_WORKER_ID = uint64_t(1) << WORKER_ID_BITS};
    enum { MAX_SEQUENCE  = uint64_t(1) << SEQUENCE_BITS};

    enum { SEQUENCE_MASK = MAX_SEQUENCE - 1 };

    enum { WORKER_ID_SHIFT = SEQUENCE_BITS };
    enum { TM_SHIFT      = SEQUENCE_BITS + WORKER_ID_BITS };

    static uint64_t genTimestamp() 
    {
        struct timeval tv;
        ::gettimeofday(&tv, nullptr);
        uint64_t time = tv.tv_usec;
        time /= 1000;
        time += (static_cast<uint64_t>(tv.tv_sec) * 1000);
        return time;
    }

    int64_t nextID() 
    {
        auto timestamp = genTimestamp();
        if (timestamp < m_last_timestamp) {
            // �������ʱ��������ˣ���������id.����ϵͳʱ�䱻����
            return 0;
        }

        if (m_last_timestamp == timestamp) {
            m_sequence = (m_sequence + 1) & SEQUENCE_MASK;
            if (m_sequence == 0) {
                // ���кų������ƣ�����ȡʱ���
                timestamp = waitNextMilliseconds(m_last_timestamp);
            }
        } else {
            m_sequence = 0;
        }
        m_last_timestamp = timestamp;

        int64_t id = 0;
        // 41λʱ��
        id = timestamp << TM_SHIFT;
        // �м�10λ�ǻ���ID
        id |= m_worker_id << WORKER_ID_SHIFT;
        // ���12λ��sequenceID
        id |= m_sequence;
        return id;
    }

    static uint64_t waitNextMilliseconds(uint64_t lastTimestamp)
    {
        auto timestamp = genTimestamp();
        while (timestamp <= lastTimestamp) {
            // �µ�ʱ���Ҫ���ھɵ�ʱ�������������Чʱ���
            timestamp = genTimestamp();
            //std::cout << "wait\n";
        }
        return timestamp;
    }

};

} // zylib
