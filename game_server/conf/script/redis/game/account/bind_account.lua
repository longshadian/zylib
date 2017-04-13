
--[[
    绑定userid对应account,account_password
	KEYS[1] = userid
    KEYS[2] = account
    KEYS[3] = account_password
    return 1--成功
--]]

local userid = KEYS[1]
local account = KEYS[2]
local account_password = KEYS[2]

redis.log(redis.LOG_WARNING, string.format("bind_account %s %s %s",userid, account, account_password))

redis.pcall("HSET", "account", account, userid)
redis.pcall("HSET", "account_pwd", userid, account_password)

return 1
