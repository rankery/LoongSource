function anyumibao_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore

end

aux.RegisterItemEvent(3305126, 0, "anyumibao_CanUse")
aux.RegisterItemEvent(3305127, 0, "anyumibao_CanUse")

function I3305126_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=400 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200736, 5, 1, 8, 420)
	elseif k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303025, 5, 1, 8, 420)
	elseif k<=800 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350022, 5, 1, 8, 420)
	elseif k<=850 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, 1, 8, 420)
	elseif k<=900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360202, 5, 1, 8, 420)
	elseif k<=949 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 6000220, 5, 1, 8, 420)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 6000224, 3, 1, 8, 420)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8700032, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 297)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8700032)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8710402, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 297)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8710402)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

function I3305127_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=400 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200737, 5, 1, 8, 420)
	elseif k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303025, 5, 1, 8, 420)
	elseif k<=800 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350022, 5, 1, 8, 420)
	elseif k<=850 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, 1, 8, 420)
	elseif k<=900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360202, 5, 1, 8, 420)
	elseif k<=949 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 6000220, 5, 1, 8, 420)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 6000224, 3, 1, 8, 420)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8700033, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 297)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8700033)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8710403, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 297)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8710403)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

aux.RegisterItemEvent(3305126, 1, "I3305126_GiftBag")
aux.RegisterItemEvent(3305127, 1, "I3305127_GiftBag")

