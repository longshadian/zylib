#ifndef _WORLD_SESSION_H_
#define _WORLD_SESSION_H_

#include <functional>
#include <memory>
#include <queue>

#include <google/protobuf/message.h>

#include "GameType.h"
#include "GameMessage.h"

#include "WorldDefine.h"
#include "zylib/Future.h"

#include "msg_login.pb.h"
#include "msg_user.pb.h"

#include "QueryResult.h"

class ObjUser;

class WorldSession : public std::enable_shared_from_this<WorldSession>
{
    using RegisterUserFuture = zylib::Future<GameRegisterUserResult>;
    using LoadUserFuture     = zylib::Future<GameLoadUserResult>;

    template <typename T>
    struct FutureCB
    {
        T                       m_future;
        std::function<void()>   m_callback;
    };
public:
    WorldSession(WorldConnection conn, std::shared_ptr<WorldConnectionInfo> conn_info);
    ~WorldSession();

	void heartbeat(uint32_t diff);
    SESSION_STATE getSessionState() const;
    bool isClosed() const;

    //session被动关闭
    void closed();

    //session主动关闭
    void shutdown();

    void postGameMessageCB(GameMessageCBPtr msg);
    void sendMessage(const ::google::protobuf::Message& msg) const;

    GameUserPtr getGameUser();
    void setGameUser(GameUserPtr user);
    const WorldConnection& getConnection() const;
    const WorldConnectionInfo& getConnectionInfo() const;
public:
    void userDisconnection();

    void handleReqRegisert(GameMessage& msg);
    void handleReqLogin(GameMessage& msg);
	void handleReqReLogin(GameMessage& msg);

public:
    /************************************************************************
     * 杂项                                                                 
     ************************************************************************/
    void handleReqConsole(GameMessage& msg);
    void handleReqCustomer(GameMessage& msg);
    void handleReqCustomerReplyConfirm(GameMessage& msg);
    void handleReqBindAccount(GameMessage& msg);
    void handleReqChangeAccountPassword(GameMessage& msg);
    void handleReqChangeIcon(GameMessage& msg);
	void handleReqExchange(GameMessage& msg);
	void handleReqBindZfb(GameMessage& msg);
	void handleReqExchangeRecord(GameMessage& msg);

public:
	/************************************************************************
	* 扎金花
	************************************************************************/
	void handleReqZjhEnterRoom(GameMessage& msg);		//请求进入房间
	void handleReqZjhExitRoom(GameMessage& msg);		//退出房间
	void handleReqZjhChangeTable(GameMessage& msg);		//请求换桌
	void handleReqZjhReady(GameMessage& msg);			//请求准备
	void handleReqZjhCard(GameMessage& msg);			//请求牌操作
	void handleReqZjhBet(GameMessage& msg);				//请求加倍
	void handleReqZjhPk2(GameMessage& msg);				//请求比牌
	void handleReqZjhPkAll(GameMessage& msg);			//请求PK所有玩家
	void handleReqChat(GameMessage& msg);				//聊天

private:
    void registerUser(std::shared_ptr<pt::req_register> req);
    void sendRspRegisterError(pt::rsp_register::RET ret);
    void sendRspLoginError(pt::rsp_login::RET ret);
	void sendRspReLoginError(const GameUser& user, pt::rsp_relogin::RET ret);

    bool checkLogin(const GameUser& user, const pt::req_login& req);
    bool checkAccountLocked(const GameUser& user);
    bool checkPassword(const GameUser& user, const pt::req_login& req);

    void registerFinish(uint64_t userid);
    void loginFinish(uint64_t userid, pt::req_login req);
private:
    WorldConnection                      m_conn;
    std::shared_ptr<WorldConnectionInfo> m_conn_info;
    SESSION_STATE                        m_state;
    std::deque<GameMessageCBPtr>         m_msg_cbs;
    GameUserPtr                          m_user;

    FutureCB<RegisterUserFuture>    m_register_result;
    FutureCB<LoadUserFuture>        m_login_result;
};

#endif
