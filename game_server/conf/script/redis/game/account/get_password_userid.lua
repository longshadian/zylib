
--[[
    获取account对应password,userid
	KEYS[1] = account
    return {password, userid}
--]]

local account = KEYS[1]

redis.log(redis.LOG_WARNING, string.format("get_password_userid %s",account))

local ret = {}
local userid = redis.pcall("HGET", "account", account)
if not userid then
    return {"", 0}
end

local password = redis.pcall("HGET", "account_pwd", userid)
if not password then
    return {"", 0}
end

table.insert(ret, password)
table.insert(ret, userid)

return ret
