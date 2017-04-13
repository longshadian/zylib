
--[[
	删除用户数据
	KEYS[1] userid
	return 	1--成功
            0--失败
--]]

redis.log(redis.LOG_WARNING, string.format("del_user_data [%s]", KEYS[1]))

local userid = KEYS[1]

----删除保存时间戳
redis.pcall("HDEL", "user_tm_save", userid)

----删除更新时间戳
redis.pcall("HDEL", "user_tm", userid)

----删除用户数据
local key = string.format("user:%s", userid)
redis.pcall("DEL", key)

return 1
