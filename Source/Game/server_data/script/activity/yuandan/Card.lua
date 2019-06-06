

function I3000014_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local Target_MapID, Target_InstanceID = role.GetRoleMapID(TargetID)
	role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3000015, 1, -1, 8, 420)
	role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
end

function I3000014_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		bRet = 32
		return bRet, bIgnore
	end
end
function I3000015_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I3000015_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(7)
	if k == 1 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350501,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	elseif k == 2 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350502,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	elseif k == 3 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350503,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	elseif k == 4 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350504,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	elseif k == 5 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350505,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	elseif k == 6 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350506,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	elseif k == 7 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1350507,1,-1,8,420)
		role.AddRoleExp(MapID, InstanceID, RoleID, Level*25)
	end
end



aux.RegisterItemEvent(3000014, 1, "I3000014_OnUse")
aux.RegisterItemEvent(3000014, 0, "I3000014_CanUse")
aux.RegisterItemEvent(3000015, 1, "I3000015_OnUse")
aux.RegisterItemEvent(3000015, 0, "I3000015_CanUse")
