
--[[
	保存user数据
	KEYS[1] userid
	KEYS[2] 时间戳
	ARGV    需要保存的玩家数据
	return 	1--成功
--]]

redis.log(redis.LOG_WARNING, string.format("save_user [%s] [%s]", KEYS[1], KEYS[2]))

local argv_len = #ARGV[1]
if argv_len < 0 then
    return 0
end

----保存时间戳
redis.pcall("HSET", "user_tm", KEYS[1], KEYS[2])

----保存用户数据
local key = string.format("user:%s",KEYS[1])
for i=1, argv_len, 2 do
    if ARGV[i] and ARGV[i+1] then
        redis.log(redis.LOG_WARNING, string.format("save_user [%s]", ARGV[i]))
        redis.pcall("HSET", key, ARGV[i], ARGV[i+1])
    end
end
return 1