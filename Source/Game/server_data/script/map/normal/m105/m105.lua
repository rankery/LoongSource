--需求4：进入场景新手修炼场[m105]，获得buff新手修炼场[2021301]

function m105_OnPlayerEnter(MapID, InstanceID,RoleID)
	--unit.AddBuff(MapID, InstanceID,RoleID, 2021301,RoleID)
	local EnterType = role.GetRoleScriptData(RoleID, 1, RoleDataType["XSXLC_EnterType"])
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20213)
	if bhave == false then
		if EnterType == 0 then
			unit.AddBuff(MapID, InstanceID, RoleID, 2021301, RoleID)
		else
			unit.AddBuff(MapID, InstanceID, RoleID, 2021302, RoleID)
		end
	end
end

--注册
aux.RegisterMapEvent("m105", 2, "m105_OnPlayerEnter")

function m105_CanSetSafeguard(MapID, InstanceID)
    return false
end

aux.RegisterMapEvent("m105", 19, "m105_CanSetSafeguard")


function m105_OnPlayerLeave(MapID, InstanceID, RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20202)
	if bhave == true then
	    unit.CancelBuff(MapID, InstanceID, RoleID, 2020201, RoleID)
		unit.AddBuff(MapID, InstanceID, RoleID, 2024701, RoleID)
	end
end

--注册
aux.RegisterMapEvent("m105", 3, "m105_OnPlayerLeave")

--玩家复活脚本
function m105_RoloRevive(MapID, InstanceID, RoleID, Type, ReviveHP, ReviveMP, x, y, z, RebornMapID)


	if Type == 0 then
		ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
		ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)
		return ReviveHP, ReviveMP, 500, 40, 500, 2291721073
	end
	return ReviveHP, ReviveMP, x, y, z, MapID
end

aux.RegisterMapEvent("m105", 11, "m105_RoloRevive")
