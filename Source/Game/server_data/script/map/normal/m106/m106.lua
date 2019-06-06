--需求4：进入场景离珠界[m106]，获得buff离珠界修炼[2021401]

function m106_OnPlayerEnter(MapID, InstanceID,RoleID)
	--unit.AddBuff(MapID, InstanceID,RoleID, 2021401,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20214)
	if bhave == false then
	unit.AddBuff(MapID, InstanceID, RoleID, 2021401, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m106", 2, "m106_OnPlayerEnter")

function m106_CanSetSafeguard(MapID, InstanceID)
    return false
end

aux.RegisterMapEvent("m106", 19, "m106_CanSetSafeguard")


function m106_OnPlayerLeave(MapID, InstanceID, RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20202)
	if bhave == true then
	    unit.CancelBuff(MapID, InstanceID, RoleID, 2020201, RoleID)
		unit.AddBuff(MapID, InstanceID, RoleID, 2024701, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m106", 3, "m106_OnPlayerLeave")

--玩家复活脚本
function m106_RoloRevive(MapID, InstanceID, RoleID, Type, ReviveHP, ReviveMP, x, y, z, RebornMapID)

	if Type == 0 then
		ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
		ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)
	    return ReviveHP, ReviveMP, 500, 40, 500, 2291720305
	end
	return ReviveHP, ReviveMP, x, y, z, MapID
end

aux.RegisterMapEvent("m106", 11, "m106_RoloRevive")
