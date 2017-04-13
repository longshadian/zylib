
--[[
    获取时间戳对应的userid
    KEYS[1] 类型 0-更新时间戳和保存时间时间戳相同
                1-更新时间戳和保存时间时间戳不同
                2-所有更新时间戳
            
    获取更新时间和保存时间不同的userid,并返回更新时间戳
	return 	{userid,更新时间戳}
--]]

local tm_type = KEYS[1]
redis.log(redis.LOG_WARNING, string.format("get_tm %s", tm_type))

local ret = {}
tm_type = tonumber(tm_type)

----获取所有更新时间戳
local all_tm = redis.pcall("HGETALL", "user_tm")

if tm_type == 0 then
    for idx = 1, #all_tm, 2 do
        local userid = all_tm[idx]
        local tm = all_tm[idx+1]
        
        ----保存时间戳和更新时间戳相同
        local tm_save = redis.pcall("HGET", "user_tm_save", userid)
        if tm_save and tm == tm_save then
            table.insert(ret, userid)
            table.insert(ret, tm)
        end
    end
elseif tm_type == 1 then 
    for idx = 1, #all_tm, 2 do
        local userid = all_tm[idx]
        local tm = all_tm[idx+1]
        
        ----不存在保存时间戳或者更新时间戳和保存时间戳不同
        local tm_save = redis.pcall("HGET", "user_tm_save", userid)
        if not tm_save or tm_save ~= tm then
            table.insert(ret, userid)
            table.insert(ret, tm)
        end
    end
elseif tm_type == 2 then
    ret = all_tm
end

return ret
