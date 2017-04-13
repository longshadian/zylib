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

    //session�����ر�
    void closed();

    //session�����ر�
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
     * ����                                                                 
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
	* ����
	************************************************************************/
	void handleReqZjhEnterRoom(GameMessage& msg);		//������뷿��
	void handleReqZjhExitRoom(GameMessage& msg);		//�˳�����
	void handleReqZjhChangeTable(GameMessage& msg);		//������
	void handleReqZjhReady(GameMessage& msg);			//����׼��
	void handleReqZjhCard(GameMessage& msg);			//�����Ʋ���
	void handleReqZjhBet(GameMessage& msg);				//����ӱ�
	void handleReqZjhPk2(GameMessage& msg);				//�������
	void handleReqZjhPkAll(GameMessage& msg);			//����PK�������
	void handleReqChat(GameMessage& msg);				//����

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
