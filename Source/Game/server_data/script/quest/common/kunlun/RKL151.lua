--RKL151
--需求1：使用巨兽骨盒2615051获得妖魂粉末2615052

--可否使用函数
function i2615051_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    bRet = 43
	else
	--如果不在指定地点，则不能使用
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 2292 or x > 2392 or z < 1443 or z > 1543 then
	          bRet = 43 
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615051_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.QuestAddRoleItem(MapID, InstanceID, TargetID, 15151, 2615052, 1, 0, 102)
end

--注册
aux.RegisterItemEvent(2615051, 1, "i2615051_QuestUsable")
aux.RegisterItemEvent(2615051, 0, "i2615051_CanUse")