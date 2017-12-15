#pragma once

#include <memory>

#include "net/NetworkType.h"

class ClientSession
{
public:
    ClientSession();
    ~ClientSession();
    ClientSession(const ClientSession& rhs) = delete;
    ClientSession& operator=(const ClientSession& rhs) = delete;
    ClientSession(ClientSession&& rhs) = delete;
    ClientSession& operator=(ClientSession&& rhs) = delete;

    uint64_t            GetUserID() const;
    void                SetUserID(uint64_t user_id);

private:
    uint64_t    m_user_id;
    Hdl         m_hdl;
};

using ClientSessionPtr = std::shared_ptr<ClientSession>;
