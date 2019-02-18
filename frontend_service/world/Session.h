#pragma once

#include <memory>

#include "net/NetworkType.h"

class ClientSession
{
public:
    ClientSession(Hdl hdl, ConnID conn_id);
    ~ClientSession();
    ClientSession(const ClientSession& rhs) = delete;
    ClientSession& operator=(const ClientSession& rhs) = delete;
    ClientSession(ClientSession&& rhs) = delete;
    ClientSession& operator=(ClientSession&& rhs) = delete;

    uint64_t            GetUserID() const;
    void                SetUserID(uint64_t user_id);

private:
    Hdl         m_hdl;
    ConnID      m_conn_id;
    uint64_t    m_user_id;
};

using ClientSessionPtr = std::shared_ptr<ClientSession>;
