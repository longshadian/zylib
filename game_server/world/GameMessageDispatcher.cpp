#include "GameMessageDispatcher.h"

#include "ProtoMsg.h"
#include "WorldSession.h"

GameMessageDispatcher::GameMessageDispatcher()
{
    m_slots = {
        {int32_t(share::MSG_ID::REQ_REGISTER),  SESSION_STATE::UNLOGIN,     &WorldSession::handleReqRegisert},
        {int32_t(share::MSG_ID::REQ_LOGIN),     SESSION_STATE::UNLOGIN,     &WorldSession::handleReqLogin},
		{int32_t(share::MSG_ID::REQ_RELOGIN),   SESSION_STATE::LOGIN,		&WorldSession::handleReqReLogin},
        {int32_t(share::MSG_ID::REQ_CONSOLE),   SESSION_STATE::LOGIN,       &WorldSession::handleReqConsole},
        {int32_t(share::MSG_ID::REQ_BIND_ACCOUNT),SESSION_STATE::LOGIN,     &WorldSession::handleReqBindAccount},
        {int32_t(share::MSG_ID::REQ_CHANGE_ACCOUNT_PASSWORD),SESSION_STATE::LOGIN,     &WorldSession::handleReqChangeAccountPassword},
        {int32_t(share::MSG_ID::REQ_CUSTOMER),SESSION_STATE::LOGIN,     &WorldSession::handleReqCustomer},
        {int32_t(share::MSG_ID::REQ_CUSTOMER_REPLY_CONFIRM),SESSION_STATE::LOGIN,     &WorldSession::handleReqCustomerReplyConfirm},
        {int32_t(share::MSG_ID::REQ_CHANGE_ICON),		SESSION_STATE::LOGIN,     &WorldSession::handleReqChangeIcon},
		{int32_t(share::MSG_ID::REQ_EXCHANGE),			SESSION_STATE::LOGIN,     &WorldSession::handleReqExchange},
		{int32_t(share::MSG_ID::REQ_BIND_ZFB),			SESSION_STATE::LOGIN,     &WorldSession::handleReqBindZfb},
		{int32_t(share::MSG_ID::REQ_EXCHANGE_RECORD),	SESSION_STATE::LOGIN,     &WorldSession::handleReqExchangeRecord },

		{ int32_t(share::MSG_ID::REQ_ZJH_ENTER_ROOM),   SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhEnterRoom },
		{ int32_t(share::MSG_ID::REQ_ZJH_EXIT_ROOM),    SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhExitRoom },
		{ int32_t(share::MSG_ID::REQ_ZJH_READY),		SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhReady },
		{ int32_t(share::MSG_ID::REQ_ZJH_CARD),			SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhCard },
		{ int32_t(share::MSG_ID::REQ_ZJH_BET),			SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhBet },
		{ int32_t(share::MSG_ID::REQ_ZJH_PK2),			SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhPk2 },
		{ int32_t(share::MSG_ID::REQ_ZJH_PKALL),		SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhPkAll },
		{ int32_t(share::MSG_ID::REQ_ZJH_CHANGE_TABLE), SESSION_STATE::LOGIN,     &WorldSession::handleReqZjhChangeTable },
		{ int32_t(share::MSG_ID::REQ_ZJH_CHAT),			SESSION_STATE::LOGIN,     &WorldSession::handleReqChat },
    };
}

GameMessageDispatcher& GameMessageDispatcher::getInstance()
{
    static GameMessageDispatcher instance_;
    return instance_;
}

bool GameMessageDispatcher::init()
{
    for (const auto& it : m_slots) {
        m_handles.insert({it.m_cmd, {it.m_state, it.m_fun}});
    }
    return true;
}

WorldSessionCB GameMessageDispatcher::findRegCallback(int32_t cmd) const
{
    auto it = m_handles.find(cmd);
    if (it == m_handles.end()) {
        return {SESSION_STATE::UNLOGIN, nullptr};
    }
    return it->second;
}
