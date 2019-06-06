--需求4：进入场景折震珠界[m108]，获得buff震珠界修炼[2030001]
--2030001	震珠界修炼
--4100101	无法换装

function m108_OnPlayerEnter(MapID, InstanceID, RoleID)
	--unit.AddBuff(MapID, InstanceID,RoleID, 2021501,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20300)
	local bhave_huanyi = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 41001)
	if bhave == false then
	unit.AddBuff(MapID, InstanceID, RoleID, 2030001, RoleID)
	end
	--添加无法换装BUFF
	if	bhave_huanyi == false then
	unit.AddBuff(MapID, InstanceID, RoleID, 4100101, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m108", 2, "m108_OnPlayerEnter")

function m108_CanSetSafeguard(MapID, InstanceID)
    return false
end

aux.RegisterMapEvent("m108", 19, "m108_CanSetSafeguard")

function m108_OnPlayerLeave(MapID, InstanceID, RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20202)
	local bhave_huanyi = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 41001)
	if bhave == true then
	    unit.CancelBuff(MapID, InstanceID, RoleID, 2020201, RoleID)
		unit.AddBuff(MapID, InstanceID, RoleID, 2024701, RoleID)
	end
	if bhave_huanyi == true then
		unit.CancelBuff(MapID, InstanceID, RoleID, 4100101, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m108", 3, "m108_OnPlayerLeave")

--玩家复活脚本
function m108_RoloRevive(MapID, InstanceID, RoleID, Type, ReviveHP, ReviveMP, x, y, z, RebornMapID)

	if Type == 0 then
		ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
		ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)
	    return ReviveHP, ReviveMP, 526, 160, 554, 2291723889
	end
	return ReviveHP, ReviveMP, x, y, z, MapID
end

aux.RegisterMapEvent("m108", 11, "m108_RoloRevive")
