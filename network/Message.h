#pragma once

#include <vector>
#include <chrono>

struct Message
{
    const uint8_t* getMsgData() const { return m_msg_pos; }
    int32_t getMsgLength() const { return m_msg_length; }

    void setMsgData(const uint8_t* pos) { m_msg_pos = pos; }
    void setMsgLength(int32_t len) { m_msg_length = len; }

    std::chrono::system_clock::time_point m_timestamp;

    int32_t     m_head_len;
    uint32_t    m_key;
    int32_t     m_msg_id;
    std::vector<uint8_t> m_body_data;
private:
    const uint8_t* m_msg_pos;
    int32_t m_msg_length;
};
