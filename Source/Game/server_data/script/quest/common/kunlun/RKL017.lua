--RKL017
--使用处理过的异香果[2615003]，刷新 赤火狐[1009003]
--2423-70<x<2423+70,2374-70<z<2374+70

--可否使用函数
function i2615003_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    bRet = 43
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 2353 or x > 2493 or z < 2304 or z > 2444 then
	          bRet = 43
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615003_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
       map.MapCreateCreature(MapID, InstanceID, 1009003, x, y, z)
end

--注册
aux.RegisterItemEvent(2615003, 1, "i2615003_QuestUsable")
aux.RegisterItemEvent(2615003, 0, "i2615003_CanUse")
