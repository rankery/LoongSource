function sishengshoubaoxiang_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore

end

aux.RegisterItemEvent(3305113, 0, "sishengshoubaoxiang_CanUse")
aux.RegisterItemEvent(3305114, 0, "sishengshoubaoxiang_CanUse")

function I3305113_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200736, 5, 1, 8, 420)
	elseif k<=600 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303028, 5, 1, 8, 420)
	elseif k<=850 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000309, 1, 1, 8, 420)
	elseif k<=950 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, 1, 8, 420)
	elseif k<=968 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310152, 1, 1, 8, 420)
	elseif k<=986 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310154, 1, 1, 8, 420)
	elseif k<=991 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3305014, 1, 1, 8, 420)
	elseif k<=996 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3305015, 1, 1, 8, 420)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907703, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 293)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907703)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907701, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 293)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907701)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

function I3305114_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200737, 5, 1, 8, 420)
	elseif k<=600 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303028, 5, 1, 8, 420)
	elseif k<=850 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000309, 1, 1, 8, 420)
	elseif k<=950 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303031, 1, 1, 8, 420)
	elseif k<=968 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310157, 1, 1, 8, 420)
	elseif k<=986 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310154, 1, 1, 8, 420)
	elseif k<=991 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3305014, 1, 1, 8, 420)
	elseif k<=996 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3305015, 1, 1, 8, 420)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907704, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 293)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907704)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907702, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 293)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907702)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

aux.RegisterItemEvent(3305113, 1, "I3305113_GiftBag")
aux.RegisterItemEvent(3305114, 1, "I3305114_GiftBag")

