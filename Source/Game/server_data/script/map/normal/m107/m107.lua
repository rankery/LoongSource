--需求4：进入场景坎珠界[m107]，获得buff坎珠界修炼[2021501]

function m107_OnPlayerEnter(MapID, InstanceID,RoleID)
	--unit.AddBuff(MapID, InstanceID,RoleID, 2021501,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20215)
	if bhave == false then
	unit.AddBuff(MapID, InstanceID, RoleID, 2021501, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m107", 2, "m107_OnPlayerEnter")

function m107_CanSetSafeguard(MapID, InstanceID)
    return false
end

aux.RegisterMapEvent("m107", 19, "m107_CanSetSafeguard")


function m107_OnPlayerLeave(MapID, InstanceID, RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20202)
	if bhave == true then
	    unit.CancelBuff(MapID, InstanceID, RoleID, 2020201, RoleID)
		unit.AddBuff(MapID, InstanceID, RoleID, 2024701, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m107", 3, "m107_OnPlayerLeave")

--玩家复活脚本
function m107_RoloRevive(MapID, InstanceID, RoleID, Type, ReviveHP, ReviveMP, x, y, z, RebornMapID)

	if Type == 0 then
		ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
		ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)
	    return ReviveHP, ReviveMP, 499, 0, 448, 2291720561
	end
	return ReviveHP, ReviveMP, x, y, z, MapID
end

aux.RegisterMapEvent("m107", 11, "m107_RoloRevive")
