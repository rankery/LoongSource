function I3303125_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.RemoveFromRole(MapID, InstanceID, TargetID, 9140121, 1, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140122, 1, -1, 8, 420)
end

function I3303125_CanUse(MapID, InstanceID, TypeID, TargetID)

	local a = role.GetRoleItemNum(TargetID, 9140121)
	if a < 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1350)--您没有7天制虎头帽，无法完成升级
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 32, false
	end

	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		return 40, false
	end
end

aux.RegisterItemEvent(3303125, 1, "I3303125_OnUse")
aux.RegisterItemEvent(3303125, 0, "I3303125_CanUse")
