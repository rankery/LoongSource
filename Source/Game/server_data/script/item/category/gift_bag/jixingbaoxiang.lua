function jixingbaoxiang_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore

end

aux.RegisterItemEvent(3305115, 0, "jixingbaoxiang_CanUse")
aux.RegisterItemEvent(3305116, 0, "jixingbaoxiang_CanUse")

function I3305115_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=400 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200736, 5, 1, 8, 420)
	elseif k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303028, 5, 1, 8, 420)
	elseif k<=800 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350022, 5, 1, 8, 420)
	elseif k<=850 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, 1, 8, 420)
	elseif k<=900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360202, 5, 1, 8, 420)
	elseif k<=949 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310152, 1, 1, 8, 420)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310154, 1, 1, 8, 420)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907705, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 296)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907705)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907706, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 296)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907706)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

function I3305116_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=400 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200737, 5, 1, 8, 420)
	elseif k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303028, 5, 1, 8, 420)
	elseif k<=800 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350022, 5, 1, 8, 420)
	elseif k<=850 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, 1, 8, 420)
	elseif k<=900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360202, 5, 1, 8, 420)
	elseif k<=949 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310157, 1, 1, 8, 420)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310154, 1, 1, 8, 420)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907707, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 296)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907707)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907708, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 296)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907708)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

aux.RegisterItemEvent(3305115, 1, "I3305115_GiftBag")
aux.RegisterItemEvent(3305116, 1, "I3305116_GiftBag")

