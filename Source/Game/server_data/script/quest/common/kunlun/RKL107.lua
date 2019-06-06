--RKL107
--需求1：使用戾沌兽血2615029获得刷出怪物1009033

--可否使用函数
function i2615029_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    bRet = 43
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 2300 or x > 2400 or z < 1580 or z > 1680 then
	          bRet = 43 
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615029_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       map.MapCreateCreature(MapID, InstanceID, 1009033, 2348, 7441, 1628)
end

--注册
aux.RegisterItemEvent(2615029, 1, "i2615029_QuestUsable")
aux.RegisterItemEvent(2615029, 0, "i2615029_CanUse")