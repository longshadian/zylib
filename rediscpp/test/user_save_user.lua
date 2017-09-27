
--[[
	保存user数据
	KEYS[1] userid
	KEYS[2] 时间戳
	ARGV    需要保存的玩家数据
	return 	1--成功
--]]

local user_id = KEYS[1]
local tm      = KEYS[2]

redis.log(redis.LOG_NOTICE, string.format("user_save_user user_id: %s tm: %s",user_id, tm))

local argv_len = #ARGV
if argv_len == 0 then
    redis.log(redis.LOG_WARNING, string.format("user_save_user argv_len == 0. user_id: %s",user_id))
    return 0
end

if argv_len%2 ~= 0 then
    redis.log(redis.LOG_WARNING, string.format("user_save_user argv_len%2 != 0. user_id: %s argv_len: %s",user_id, argv_len))
    return 0
end

----保存时间戳
redis.pcall("HSET", "user_new", user_id, tm)
redis.pcall("HSET", "user_all", user_id, tm)

----保存用户数据
local key = string.format("user:%s", user_id)
for i=1, argv_len, 2 do
    redis.pcall("HSET", key, ARGV[i], ARGV[i+1])
    redis.log(redis.LOG_NOTICE, string.format("user_save_user %s %s", key, ARGV[i]))
end
return 1
