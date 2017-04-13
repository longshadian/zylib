#include "WorldSession.h"

#include "zylib/Random.h"

#include <google/protobuf/util/json_util.h>
#include "msg_other.pb.h"

#include "GameObj.h"
#include "GameAssert.h"
#include "GameMessage.h"
#include "GameConnection.h"
#include "GameUser.h"

#include "AppLog.h"
#include "ProtoMsg.h"
#include "GameObj.h"
#include "QueryResult.h"
#include "ShareDefine.h"
#include "HallResponse.h"
#include "Config.h"
#include "WorldManager.h"
#include "MiscManager.h"

#include "GlobalService.h"
#include "NetworkService.h"
#include "UseridService.h"
#include "AccountService.h"
#include "UseridService.h"
#include "UserService.h"
#include "CustomerService.h"

#include "GlobalConf.h"
#include "GameConf.h"

#include "ZjhRoom.h"
#include "ZjhDefine.h"
#include "ZjhRoomService.h"


WorldSession::WorldSession(WorldConnection conn, std::shared_ptr<WorldConnectionInfo> conn_info)
	: m_conn(conn)
    , m_conn_info(conn_info)
    , m_state(SESSION_STATE::UNLOGIN)
    , m_msg_cbs()
    , m_user(nullptr)
    , m_register_result()
    , m_login_result()
{
}

WorldSession::~WorldSession()
{
}

SESSION_STATE WorldSession::getSessionState() const { return m_state; }

void WorldSession::heartbeat(uint32_t diff)
{
    (void)diff;

    //register完成后回调
    if (m_state == SESSION_STATE::WAIT_REGISTER_FINISH && m_register_result.m_future.isReady()) {
        m_state = SESSION_STATE::UNLOGIN;
        if (m_register_result.m_callback)
            m_register_result.m_callback();
        m_register_result.m_callback = {};
    }

    //login完成后回调
    if (m_state == SESSION_STATE::WAIT_LOGIN_FINISH && m_login_result.m_future.isReady()) {
        m_state = SESSION_STATE::UNLOGIN;
        if (m_login_result.m_callback)
            m_login_result.m_callback();
        m_login_result.m_callback = {};
    }

    while (!isClosed() && !m_msg_cbs.empty()) {
        auto msg_cb = std::move(m_msg_cbs.front());
        m_msg_cbs.pop_front();
        if (msg_cb)
            msg_cb->visitor(this);
    }
    if (m_user)
        m_user->saveChange();
}

bool WorldSession::isClosed() const
{
    return m_state == SESSION_STATE::CLOSED;
}

void WorldSession::shutdown()
{
    if (!isClosed()) {
        appLog(LOG_DEBUG, "xxxxxxxxxxxxxxxxx shutdown");
        WorldManager::instance().shutdownSession(this);
    }
    closed();
}

void WorldSession::closed()
{
    m_state = SESSION_STATE::CLOSED;
}

void WorldSession::postGameMessageCB(GameMessageCBPtr msg)
{
    m_msg_cbs.push_back(std::move(msg));
}

void WorldSession::userDisconnection()
{
	if (!m_user)
		return;
    m_user->disConnection();
}

void WorldSession::handleReqRegisert(GameMessage& msg)
{
    auto p_req = std::make_shared<pt::req_register>();
    pt::req_register& req = *p_req;
    PARSE_REQ(req, msg.m_data);
    if (req.channel().size() > share::USER_MAX_CHANNEL
        || req.imsi().size() > share::USER_MAX_IMSI
        || req.imei().size() > share::USER_MAX_IMEI
        || req.phone_type().size() > share::USER_MAX_PHONE_TYPE
        || req.apkversion().size() > share::USER_MAX_VERSION
        ) {
        appLog(LOG_ERROR, "req size too long channel:%d imsi:%d imei:%d phone_type:%d apkversion:%d",
            (int32_t)req.channel().size(), (int32_t)req.imsi().size(), (int32_t)req.imei().size(),
            (int32_t)req.phone_type().size(), (int32_t)req.apkversion().size());
        return;
    }

    appLog(LOG_DEBUG, "channel:[%s] imsi:[%s] imei:[%s] phone_type:[%s] apkvserion:[%s]",
         req.channel().c_str(), req.imsi().c_str(), req.imei().c_str(), req.phone_type().c_str(), req.apkversion().c_str()
        );

    //模拟器可以无限注册
    if ((req.imei() == GlobalConf::getGameConf().getString(GAME_CONF::WIN32_IMEI) && req.imsi() == GlobalConf::getGameConf().getString(GAME_CONF::WIN32_IMSI))
        || (req.imei().empty() && req.imsi().empty())
        ) {
        registerUser(p_req);
        return;
    }

    auto device_id = AccountService::makeDeviceid(req.imei(), req.imsi());
    if (GlobalService::getAccountService().deviceIsRegistering(device_id)) {
        //该设备正在注册账号，此次注册失败。
        sendRspRegisterError(pt::rsp_register::RET_ERROR);
        appLog(LOG_ERROR, "device is registering");
        return;
    }

    auto device = GlobalService::getAccountService().findDevice(device_id);
    if (device) {
        auto unbind_account = device->getUnbindAccount();
        if (unbind_account) {
            //1.有已注册未绑定的账号，返回该账号
            pt::rsp_register rsp{};
            rsp.set_ret(pt::rsp_register::RET_SUCCESS);
            rsp.set_userid(unbind_account->m_userid);
            rsp.set_password(unbind_account->m_password);
            sendMessage(rsp);
            return;
        }

        //判断注册账号数是否超出
        auto count = device->accountCount();
        auto max_count = (size_t)GlobalConf::getGameConf().getInt32(GAME_CONF::REGISTER_ACCOUNT);
        if (count >= max_count) {
            auto last_login_account = device->getLastLoginAccount();
            pt::rsp_register rsp{};
            rsp.set_ret(pt::rsp_register::RET_REG_TOO_MUCH);
            rsp.set_bind_account(last_login_account->m_account);
            sendMessage(rsp);
            return;
        }
    }

    registerUser(p_req);
    return;
}

void WorldSession::handleReqLogin(GameMessage& msg)
{
    pt::req_login req{};
    PARSE_REQ(req, msg.m_data);

    uint64_t userid = 0;
    if (req.login_type() == pt::req_login::TYPE_USERID) {
        userid = req.userid();
    } else if (req.login_type() == pt::req_login::TYPE_BIND_ACCOUNT) {
        std::string account_pwd{};
        if (GlobalService::getAccountService().getAccountPwdAndUserid(req.bind_account(), &account_pwd, &userid)) {
            if (account_pwd != req.bind_account_pwd()) {
                sendRspLoginError(pt::rsp_login::RET_PASSWORD_ERROR);
                return;
            }
        } else {
            sendRspLoginError(pt::rsp_login::RET_ACCOUNT_NULL);
            return;
        }
    }

    if (userid == 0) {
        sendRspLoginError(pt::rsp_login::RET_ERROR);
        return;
    }
    appLog(LOG_DEBUG, "login %" PRIu64, userid);

    GameUserPtr login_user = nullptr;
    //1.从系统临时代理中获取
    login_user = GlobalService::getUserService().findAgent(userid);
    if (login_user) {
        if (checkLogin(*login_user, req)) {
            m_user = login_user;
            m_user->setSession(this);
            GlobalService::getUserService().removeAgent(userid);
            GlobalService::getUserService().addOnline(userid, shared_from_this());
            m_user->login(req);
            m_state = SESSION_STATE::LOGIN;
        } else {
            //登录失败
            appLog(LOG_DEBUG, "req login fail %" PRIu64, userid);
        }
        return;
    }

    //2.检查该帐号是否已经在线
    auto other_session = GlobalService::getUserService().findOnline(userid);
    if (other_session) {
        login_user = other_session->getGameUser();
        if (checkLogin(*login_user, req)) {
            //新登录的玩家顶替之前登录的玩家
            //2.1.发送异地登录信息
            pt::rsp_account_exception rsp_ae{};
            rsp_ae.set_ret(pt::rsp_account_exception::RET_REPEATED_LOGIN);
            other_session->sendMessage(rsp_ae);

            //2.2.游戏中断开连接
            other_session->userDisconnection();
            other_session->setGameUser(nullptr);

            //2.3.session断开连接
            other_session->shutdown();
            appLog(LOG_DEBUG, "xxx repeated login");

            //2.4新登录的玩家接管数据
            m_user = login_user;
            m_user->setSession(this);
            GlobalService::getUserService().addOnline(m_user->getUserid(), shared_from_this());

            //2.5.通知新登录的玩家登录成功
            m_user->login(req);
            m_state = SESSION_STATE::LOGIN;
        } else {
            appLog(LOG_DEBUG, "req login fail %" PRIu64, userid);
        }
        return;
    }

    //3.去缓存中取用户
    login_user = GlobalService::getUserService().findUserFromPool(userid);
    if (login_user) {
        if (checkLogin(*login_user, req)) {
            m_user = login_user;
            m_user->setSession(this);
            m_user->login(req);
            m_state = SESSION_STATE::LOGIN;
            GlobalService::getUserService().addOnline(m_user->getUserid(), shared_from_this());
        } else {
            appLog(LOG_DEBUG, "req login fail %" PRIu64, userid);
        }
        return;
    }

    //4.如果多帐号同时登录,断开后来的连接
    if (GlobalService::getUserService().isLoading(userid) || GlobalService::getUserService().isRegistering(userid)) {
        pt::rsp_account_exception rsp_ae{};
        rsp_ae.set_ret(pt::rsp_account_exception::RET_REPEATED_LOGIN);
        sendMessage(rsp_ae);
        shutdown();
        appLog(LOG_DEBUG, "xxx repeated login 2");
        return;
    }

    //5.加载用户数据，设置回调
    m_state = SESSION_STATE::WAIT_LOGIN_FINISH;
    m_login_result.m_future = GlobalService::getUserService().loadUserByFuture(userid);
    m_login_result.m_callback = std::bind(&WorldSession::loginFinish, this, req.userid(), req);
}

void WorldSession::handleReqBindAccount(GameMessage& msg)
{
    pt::req_bind_account req{};
    PARSE_REQ(req, msg.m_data);

    if (!(share::USER_MIN_BIND_ACCOUNT <= req.account().size() && req.account().size() <= share::USER_MAX_BIND_ACCOUNT)
        || !(share::USER_MIN_BIND_ACCOUNT_PWD <= req.password().size() && req.password().size() <= share::USER_MAX_BIND_ACCOUNT_PWD)
        ) {
        pt::rsp_bind_account rsp{};
        rsp.set_ret(pt::rsp_bind_account::ERROR_LENGTH);
        sendMessage(rsp);
        return;
    }
    if (std::strlen(m_user->getBindAccount()) != 0) {
        pt::rsp_bind_account rsp{};
        rsp.set_ret(pt::rsp_bind_account::ERROR_BIND_YET);
        sendMessage(rsp);
        return;
    }
    if (GlobalService::getAccountService().isBind(req.account())) {
        pt::rsp_bind_account rsp{};
        rsp.set_ret(pt::rsp_bind_account::ERROR_SAME_ACCOUNT);
        sendMessage(rsp);
        return;
    }
    if (GlobalService::getAccountService().bindUserAccount(*m_user, req.account(), req.password())) {
        pt::rsp_bind_account rsp{};
        rsp.set_ret(pt::rsp_bind_account::SUCCESS);
        sendMessage(rsp);

        auto val = GlobalConf::getGameConf().getInt64(GAME_CONF::BIND_ACCOUNT_REWARD);
        m_user->changeGold(val);
        hall::sendRspSyncUser(*m_user);
    } else {
        pt::rsp_bind_account rsp{};
        rsp.set_ret(pt::rsp_bind_account::ERROR);
        sendMessage(rsp);
    }
}

void WorldSession::handleReqChangeAccountPassword(GameMessage& msg)
{
    pt::req_change_account_password req{};
    PARSE_REQ(req, msg.m_data);

    if (!(share::USER_MIN_BIND_ACCOUNT_PWD <= req.new_password().size() && req.new_password().size() <= share::USER_MAX_BIND_ACCOUNT_PWD)
        ) {
        pt::rsp_change_account_password rsp{};
        rsp.set_ret(pt::rsp_change_account_password::ERROR_LENGTH);
        sendMessage(rsp);
        return;
    }

    //未绑定账号
    if (std::strlen(m_user->getBindAccount()) == 0) {
        pt::rsp_change_account_password rsp{};
        rsp.set_ret(pt::rsp_change_account_password::ERROR);
        sendMessage(rsp);
        return;
    }

    if (req.old_password() != m_user->getBindAccountPwd()) {
        pt::rsp_change_account_password rsp{};
        rsp.set_ret(pt::rsp_change_account_password::ERROR_OLD_PASSWORD_ERROR);
        sendMessage(rsp);
        return;
    }

    if (GlobalService::getAccountService().changeAccountPwd(*m_user, req.new_password())) {
        pt::rsp_change_account_password rsp{};
        rsp.set_ret(pt::rsp_change_account_password::SUCCESS);
        sendMessage(rsp);
        hall::sendRspSyncUser(*m_user);
    } else {
        pt::rsp_change_account_password rsp{};
        rsp.set_ret(pt::rsp_change_account_password::ERROR);
        sendMessage(rsp);
        hall::sendRspSyncUser(*m_user);
    }
}

void WorldSession::handleReqChangeIcon(GameMessage& msg)
{
	pt::req_change_icon req{};
	PARSE_REQ(req, msg.m_data);
	appLog(LOG_DEBUG, "userid:%" PRIu64,  m_user->getUserid());

    auto icon_id = static_cast<int32_t>(std::strtol(req.icon().c_str(), nullptr, 10));
    auto icon_min = GlobalConf::getGameConf().getInt32(GAME_CONF::SYSTEM_COIN_MIN);
    auto icon_max = GlobalConf::getGameConf().getInt32(GAME_CONF::SYSTEM_COIN_MAX);
    if (icon_max < icon_id || icon_id < icon_min
		|| req.icon().size() > share::USER_MAX_ICON) {
		appLog(LOG_DEBUG, "icon idx error");
        return;
    }
    m_user->setIcon(req.icon());
    hall::sendRspSyncUser(*m_user);
}

void WorldSession::handleReqExchange(GameMessage& msg)
{
	pt::req_exchange req{};
	PARSE_REQ(req, msg.m_data);

	appLog(LOG_DEBUG, "userid:%" PRIu64 " req exchange gold:%" PRId64, m_user->getUserid(), req.exchange_gold());

	if (!m_user->isBindZfbAccount()) {
		appLog(LOG_DEBUG, "no bind zfb");
		pt::rsp_exchange rsp{};
		rsp.set_ret(pt::rsp_exchange::RET_NO_BIND);
		sendMessage(rsp);
		return;
	}

	auto user_gold = m_user->getGold();
	auto exchange_gold = req.exchange_gold();
	auto exchange_gold_min = GlobalConf::getGameConf().getInt32(GAME_CONF::EXCHANGE_GOLD_MIN);
	if (exchange_gold % exchange_gold_min != 0) {
		appLog(LOG_DEBUG, "exchange gold must 50 multiple");
		pt::rsp_exchange rsp{};
		rsp.set_ret(pt::rsp_exchange::RET_ERROR);
		sendMessage(rsp);
		return;
	}

	//余额不足:1.当前账户金币<请求兑换金币  
	auto fee = exchange_gold <= 2 * exchange_gold_min ? 2*100 : exchange_gold*0.02;
	auto expend_gold = exchange_gold + fee;
	if (user_gold < expend_gold + 1000){
		pt::rsp_exchange rsp{};
		rsp.set_ret(pt::rsp_exchange::RET_GOLD_LESS);
		sendMessage(rsp);
		return;
	}

	appLog(LOG_DEBUG, "userid:%" PRIu64 " req exchange_gold:%" PRId64 " expend_gold:%d"
		, m_user->getUserid(), exchange_gold, (int32_t)expend_gold);

	m_user->changeGold(-expend_gold);
	m_user->changeWithdrawGold(exchange_gold);
	MiscManager::usrExchangeRecordAdd(m_user, exchange_gold);
	
	pt::rsp_exchange rsp{};
	rsp.set_ret(pt::rsp_exchange::RET_SUCCESS);
	sendMessage(rsp);
	
	hall::sendRspSyncUser(*m_user);
}

void WorldSession::handleReqBindZfb(GameMessage& msg)
{
	pt::req_bind_zfb req{};
	PARSE_REQ(req, msg.m_data);
	
	auto account_size = req.account_zfb().size();
	auto name_size    = req.account_name().size();

	appLog(LOG_DEBUG, "userid:%" PRIu64 " req bind zfb account_size:%d name_size:%d", m_user->getUserid(), (int32_t)account_size, (int32_t)name_size);

	if (!(share::USER_MIN_BIND_ZFB_ACCOUNT <= account_size && account_size <= share::USER_MAX_BIND_ZFB_ACCOUNT)
		|| !(share::USER_MIN_BIND_ZFB_ACCOUNT_NAME <= name_size && name_size <= share::USER_MAX_BIND_ZFB_ACCOUNT_NAME)) {
		appLog(LOG_DEBUG, "bind zfb no full condition");
		pt::rsp_bind_zfb rsp{};
		rsp.set_ret(pt::rsp_bind_zfb::RET_ACCOUNT_ERROR);
		sendMessage(rsp);
		return;
	}

	if (std::strlen(m_user->getBindZfbAccount()) != 0) {
		appLog(LOG_DEBUG, "already bind zfb");
		pt::rsp_bind_zfb rsp{};
		rsp.set_ret(pt::rsp_bind_zfb::RET_ALREADY_BIND);
		sendMessage(rsp);
		return;
	}

	m_user->setBindZfbAccount(req.account_zfb());
	m_user->setBindZfbName(req.account_name());

	pt::rsp_bind_zfb rsp{};
	rsp.set_ret(pt::rsp_bind_zfb::RET_SUCCESS);
	sendMessage(rsp);

	hall::sendRspSyncUser(*m_user);
}

void WorldSession::handleReqExchangeRecord(GameMessage& msg)
{
	pt::req_exchange_record req{};
	PARSE_REQ(req, msg.m_data);

	appLog(LOG_DEBUG, "userid:%" PRIu64 " req exchange record", m_user->getUserid());

	pt::rsp_exchange_record rsp{};
	auto& userExchange = m_user->getUserExchange();
	MiscManager::userExchangeToProto(userExchange, &rsp);
	
	sendMessage(rsp);
}

void WorldSession::handleReqReLogin(GameMessage& msg)
{
	pt::req_relogin req{};
	PARSE_REQ(req, msg.m_data);
	appLog(LOG_DEBUG, "userid:%" PRIu64 " relogin in %d-%" PRId64, m_user->getUserid(), m_user->getGameid(), m_user->getTableid());

	auto gameid = m_user->getGameid();
	if (gameid == 0) {
		sendRspReLoginError(*m_user, pt::rsp_relogin::RET_ERROR);
		return;
	}

	switch (gameid) {
	case zjh::ZJH_ROOM_EXPERIENCE:
	case zjh::ZJH_ROOM_CIVILIAN:
	case zjh::ZJH_ROOM_XIAOZI:
	case zjh::ZJH_ROOM_BOSS:
	case zjh::ZJH_ROOM_TYRANT: {
		auto room = GlobalService::getRoomService().findRoom(gameid);
		GameAssert(room);
		room->reEnterRoom(m_user);
		return;
	}
	default:
		sendRspReLoginError(*m_user, pt::rsp_relogin::RET_ERROR);
		break;
	}
}

void WorldSession::handleReqConsole(GameMessage& msg)
{
    const std::map<std::string, std::string> CMD =
    {
        {"0", "add_gold"},
        {"1", "hall_msg"},
    };
    (void)CMD;

    pt::req_console req{};
    PARSE_REQ(req, msg.m_data);

    appLog(LOG_DEBUG, "console %s", req.str().c_str());

    auto str = req.str();
    auto ret = zylib::stringSplit(str, ' ');
    if (ret.size() != 2)
        return;
    auto key = ret[0];
    auto value = ret[1];
    if (key == "0") {
        auto f = std::strtold(value.c_str(), nullptr);
        f *= 100;
        int64_t v = static_cast<int64_t>(f);
        if (v <= 0)
            return;
        m_user->changeGold(v);
		m_user->changeChargeGold(v);
        hall::sendRspSyncUser(*m_user);
    } else if (key == "1") {
        GlobalService::getUserService().broadcastHallMsg(value);
    }
}

void WorldSession::handleReqCustomer(GameMessage& msg)
{
    pt::req_customer req{};
    PARSE_REQ(req, msg.m_data);
    appLog(LOG_DEBUG, "%" PRIu64, m_user->getUserid());

    GlobalService::getCustomerService().reqCustomer(*m_user, req);
}

void WorldSession::handleReqCustomerReplyConfirm(GameMessage& msg)
{
    pt::req_customer_reply_confirm req{};
    PARSE_REQ(req, msg.m_data);
    appLog(LOG_DEBUG, "%" PRIu64, m_user->getUserid());

    GlobalService::getCustomerService().reqCustomerReplyConfirm(*m_user, req);
}

void WorldSession::sendMessage(const ::google::protobuf::Message& msg) const
{
    GlobalService::getNetworkService().sendMessage(m_conn, msg);
}

GameUserPtr WorldSession::getGameUser()
{
    return m_user;
}

void WorldSession::setGameUser(GameUserPtr user)
{
    m_user = user;
}

const WorldConnection& WorldSession::getConnection() const
{
    return m_conn;
}

const WorldConnectionInfo& WorldSession::getConnectionInfo() const
{
    return *m_conn_info;
}

void WorldSession::registerFinish(uint64_t userid)
{
    m_state = SESSION_STATE::UNLOGIN;
    GameUserPtr user = nullptr;
    try {
        auto reg_result = m_register_result.m_future.getValue();
        if (reg_result.m_result == DB_RESULT::ERROR) {
            appLog(LOG_ERROR, " register db fail %" PRIu64, userid);
            sendRspRegisterError(pt::rsp_register::RET_ERROR);
            return;
        }
        user = reg_result.m_data;
    } catch (const std::exception& e) {
        appLog(LOG_ERROR, "future exception %s", e.what());
        sendRspRegisterError(pt::rsp_register::RET_ERROR);
        return;
    }

    if (!user) {
        appLog(LOG_ERROR, " register db fail %" PRIu64, userid);
        sendRspRegisterError(pt::rsp_register::RET_ERROR);
        return;
    }

    appLog(LOG_DEBUG, "uid:%" PRIu64" passwrod:%s", userid, user->getPassword());

    pt::rsp_register rsp{};
    rsp.set_ret(pt::rsp_register::RET_SUCCESS);
    rsp.set_userid(userid);
    rsp.set_password(user->getPassword());
    sendMessage(rsp);
}
 
void WorldSession::loginFinish(uint64_t userid, pt::req_login req)
{
    m_state = SESSION_STATE::UNLOGIN;
    GameUserPtr game_user = nullptr;
    try {
        auto load_result = m_login_result.m_future.getValue();
        if (load_result.m_result == DB_RESULT::ERROR) {
            sendRspLoginError(pt::rsp_login::RET_ERROR);
            appLog(LOG_ERROR, "login db fail %" PRIu64, userid);
            return;
        }
        game_user = load_result.m_data;
    } catch (const std::exception& e) {
        sendRspLoginError(pt::rsp_login::RET_ERROR);
        appLog(LOG_ERROR, "future exception userid:%" PRIu64 " %s", userid, e.what());
        return;
    }

    if (!game_user) {
        //帐号不存在
        sendRspLoginError(pt::rsp_login::RET_ACCOUNT_NULL);
        appLog(LOG_ERROR, "account null userid:%" PRIu64, userid);
        return;
    }

    if (checkLogin(*game_user, req)) {
        m_user = game_user;
        m_state = SESSION_STATE::LOGIN;
        m_user->setSession(this);
        GlobalService::getUserService().addOnline(m_user->getUserid(), shared_from_this());
        m_user->login(req);
    } else {
        appLog(LOG_DEBUG, "login falid userid:%" PRIu64, userid);
    }
}

void WorldSession::registerUser(std::shared_ptr<pt::req_register> req)
{
    auto userid = GlobalService::getUseridService().newUserid();
    if (userid == 0) {
        sendRspRegisterError(pt::rsp_register::RET_ERROR);
        appLog(LOG_ERROR, "guid overflow");
        return;
    }

    auto obj_user = GameUser::createObjUser(userid, *req);
    if (!obj_user) {
        sendRspRegisterError(pt::rsp_register::RET_ERROR);
        appLog(LOG_ERROR, "obj_user null");
        return;
    }

    m_state = SESSION_STATE::WAIT_REGISTER_FINISH;
    m_register_result.m_future = GlobalService::getUserService().registerUserByFuture(std::move(obj_user));
    m_register_result.m_callback = std::bind(&WorldSession::registerFinish, this, userid);
}

void WorldSession::sendRspRegisterError(pt::rsp_register::RET ret)
{
    pt::rsp_register rsp{};
    rsp.set_ret(ret);
    sendMessage(rsp);
}

void WorldSession::sendRspLoginError(pt::rsp_login::RET ret)
{
    pt::rsp_login rsp{};
    rsp.set_ret(ret);
    sendMessage(rsp);
}

void WorldSession::sendRspReLoginError(const GameUser& user, pt::rsp_relogin::RET ret)
{
	pt::rsp_relogin rsp{};
	rsp.set_ret(ret);
	rsp.set_gameid(0);
	sendMessage(rsp);
}

bool WorldSession::checkLogin(const GameUser& user, const pt::req_login& req)
{
    if (!checkAccountLocked(user)) {
        appLog(LOG_DEBUG, "account locked %" PRIu64, user.getUserid());
        return false;
    }

    if (req.login_type() == pt::req_login::TYPE_USERID) {
        if (!checkPassword(user, req)) {
            appLog(LOG_DEBUG, "password error %" PRIu64, user.getUserid());
            return false;
        }
    }
    return true;
}

bool WorldSession::checkAccountLocked(const GameUser& user)
{
    if (user.getUserState() == USER_STATE::ACCOUNT_LOCKED) {
        sendRspLoginError(pt::rsp_login::RET_ACCOUNT_LOCK);
        appLog(LOG_DEBUG, "login_error %" PRIu64, user.getUserid());
        return false;
    }
    return true;
}

bool WorldSession::checkPassword(const GameUser& user, const pt::req_login& req)
{
    if (user.getPassword() != req.password()) {
        sendRspLoginError(pt::rsp_login::RET_PASSWORD_ERROR);
        appLog(LOG_ERROR, "login_error %" PRIu64, user.getUserid());
        return false;
    }
    return true;
}
