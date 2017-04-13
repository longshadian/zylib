
--[[
	检查缓存的user数据能否被删除。如果允许，删除该user数据
	KEYS[1] userid
	return 	0 	删除不成功
			1	删除成功
--]]

redis.log(redis.LOG_WARNING, string.format("try_remove [%s]", KEYS[1]))

local userid = KEYS[1]

local user_tm = redis.pcall("HGET", "user_tm", userid)
if not user_tm then
	redis.log(redis.LOG_WARNING, string.format("try_remove user_tm nil [%s]", userid))
	return 0
end

local user_tm_save = redis.pcall("HGET", "user_tm_save", userid);
if not user_tm_save then
	redis.log(redis.LOG_WARNING, string.format("try_remove user_tm_save nil [%s]", userid))
	return 0
end

if user_tm == user_tm_save then
	redis.pcall("HDEL", "user_tm", userid)
	redis.pcall("HDEL", "user_tm_save", userid)
	redis.pcall("DEL", string.format("user:%s", userid))
	redis.log(redis.LOG_WARNING, string.format("try_remove succ %s", userid))
	return 1
end
redis.log(redis.LOG_WARNING, string.format("try_remove_user unremove %s", userid))
return 0