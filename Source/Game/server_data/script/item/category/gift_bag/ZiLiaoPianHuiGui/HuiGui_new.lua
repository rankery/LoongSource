function I1330634_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Target_MapID, Target_InstanceID = role.GetRoleMapID(TargetID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local targetlevel = role.GetRoleLevel(Target_MapID, Target_InstanceID, TargetID)

	role.AddRoleItem(MapID, InstanceID, RoleID, 1340001, 2, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, RoleID, 1320001, 2, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, RoleID, 4100112, 1, -1, 8, 420)

	if rolelevel<30 then
		role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 1140001, 3, -1, 8, 420)
	elseif rolelevel <50 then
		role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 1340002, 1, -1, 8, 420)
	elseif rolelevel<80 then
		role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 1340002, 2, -1, 8, 420)
		if targetlevel>=95 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200637, 1, -1, 8, 420)
		elseif targetlevel>=85 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200636, 2, -1, 8, 420)
		elseif targetlevel>=75 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200564, 2, -1, 8, 420)
		elseif targetlevel>=65 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200563, 3, -1, 8, 420)
		elseif targetlevel>=55 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200562, 3, -1, 8, 420)
		else
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3303031, 1, -1, 8, 420)
		end
	elseif rolelevel>=80 then
		role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 1340002, 2, -1, 8, 420)
		role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 2220401, 1, -1, 8, 420)
		if targetlevel>=95 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200637, 2, -1, 8, 420)
		elseif targetlevel>=85 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200636, 3, -1, 8, 420)
		elseif targetlevel>=75 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200564, 3, -1, 8, 420)
		elseif targetlevel>=65 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200563, 4, -1, 8, 420)
		elseif targetlevel>=55 then
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3200562, 4, -1, 8, 420)
		else
			role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3303031, 1, -1, 8, 420)
		end
	end

end

function I1330634_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够

	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end

	local FreeSize = role.GetBagFreeSize(TargetID)

	local ReqSize = 0
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)

	if rolelevel<50 then
		ReqSize=1
	elseif rolelevel<80 then
		ReqSize=2
	elseif rolelevel>=80 then
		ReqSize=3
	end

	if(FreeSize < ReqSize) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		bRet = 32
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(1330634, 1, "I1330634_OnUse")
aux.RegisterItemEvent(1330634, 0, "I1330634_CanUse")


function I1330635_OnUse(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1340001, 2, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1320001, 2, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4100112, 1, -1, 8, 420)

end

function I1330635_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330635, 1, "I1330635_OnUse")
aux.RegisterItemEvent(1330635, 0, "I1330635_CanUse")


