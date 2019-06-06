--15432万修道一卷（2615217）：与二卷三卷一起使用获得万修全书
--15432万修道二卷（2615218）：与一卷三卷一起使用获得万修全书
--15432万修道三卷（2615219）：与一卷二卷一起使用获得万修全书
--15432万修全书（2615228）：到指定地点使用召唤万修道长1009219（1226,14595,274）

--可否使用函数
function i2615217_CanUse(MapID, InstanceID, TypeID, TargetID)
    local i = role.GetRoleItemNum(TargetID, 2615218)
    local k = role.GetRoleItemNum(TargetID, 2615219)
    if i >= 1 and k >= 1 then
        return 0, false
    else
        return 32, false
    end
end

--使用效果
function i2615217_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    role.RemoveFromRole(MapID, InstanceID, TargetID, 2615218, 1, 420)
    role.RemoveFromRole(MapID, InstanceID, TargetID, 2615219, 1, 420)
    role.AddRoleItem(MapID, InstanceID, TargetID, 2615228, 1, -1, 8, 420)

end

--注册
aux.RegisterItemEvent(2615217, 1, "i2615217_QuestUsable")
aux.RegisterItemEvent(2615217, 0, "i2615217_CanUse")

--可否使用函数
function i2615218_CanUse(MapID, InstanceID, TypeID, TargetID)
    local i = role.GetRoleItemNum(TargetID, 2615217)
    local k = role.GetRoleItemNum(TargetID, 2615219)
    if i >= 1 and k >= 1 then
        return 0, false
    else
        return 32, false
    end
end

--使用效果
function i2615218_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    role.RemoveFromRole(MapID, InstanceID, TargetID, 2615217, 1, 420)
    role.RemoveFromRole(MapID, InstanceID, TargetID, 2615219, 1, 420)
    role.AddRoleItem(MapID, InstanceID, TargetID, 2615228, 1, -1, 8, 420)
end

--注册
aux.RegisterItemEvent(2615218, 1, "i2615218_QuestUsable")
aux.RegisterItemEvent(2615218, 0, "i2615218_CanUse")

--可否使用函数
function i2615219_CanUse(MapID, InstanceID, TypeID, TargetID)
    local i = role.GetRoleItemNum(TargetID, 2615218)
    local k = role.GetRoleItemNum(TargetID, 2615217)
    if i >= 1 and k >= 1 then
        return 0, false
    else
        return 32, false
    end
end

--使用效果
function i2615219_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    role.RemoveFromRole(MapID, InstanceID, TargetID, 2615218, 1, 420)
    role.RemoveFromRole(MapID, InstanceID, TargetID, 2615217, 1, 420)
    role.AddRoleItem(MapID, InstanceID, TargetID, 2615228, 1, -1, 8, 420)

end

--注册
aux.RegisterItemEvent(2615219, 1, "i2615219_QuestUsable")
aux.RegisterItemEvent(2615219, 0, "i2615219_CanUse")

--可否使用函数
function i2615228_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    bRet = 43
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 1156 or x > 1296 or z < 204 or z > 344 then
	          bRet = 43
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615228_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       map.MapCreateColCreature(MapID, InstanceID, 1009219, 1226, 14595, 274, 1, "")
end

--注册
aux.RegisterItemEvent(2615228, 1, "i2615228_QuestUsable")
aux.RegisterItemEvent(2615228, 0, "i2615228_CanUse")
