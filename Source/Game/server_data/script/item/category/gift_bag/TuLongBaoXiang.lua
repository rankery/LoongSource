function I3305109_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local r = math.random(1000)

	if r<=340 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350020, 5, -1, 8, 420)

	elseif r<=680 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350021, 5, -1, 8, 420)

	elseif r<=728 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2220401, 1, -1, 8, 420)

	elseif r<=776 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303025, 5, -1, 8, 420)

	elseif r<=826 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3304001, 4, -1, 8, 420)

	elseif r<=916 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3400103, 1, -1, 8, 420)

	elseif r<=964 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, -1, 8, 420)

	elseif r<=999 then

		local r2 = math.random(5)

		if TypeID==3305109 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907405+r2, 1, 5, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 293)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 8907205+r2)
			msg.DispatchWorldMsgEvent(MsgID)

		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907400+r2, 1, 5, 8, 420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 293)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 8907200+r2)
			msg.DispatchWorldMsgEvent(MsgID)

		end



	elseif r<=1000 then

		if TypeID==3305109 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907206, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907207, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907208, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907209, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907210, 1, 5, 8, 420)

		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907201, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907202, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907203, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907204, 1, 5, 8, 420)
			role.AddRoleItem(MapID, InstanceID, TargetID, 8907205, 1, 5, 8, 420)

		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 294)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.DispatchWorldMsgEvent(MsgID)
	end

end

function I3305109_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3305109, 1, "I3305109_GiftBag")
aux.RegisterItemEvent(3305110, 1, "I3305109_GiftBag")

aux.RegisterItemEvent(3305109, 0, "I3305109_CanUseGiftBag")
aux.RegisterItemEvent(3305110, 0, "I3305109_CanUseGiftBag")

