function I3303332_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local r = math.random(1000)

	if r<=2 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8600007, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 291)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8600007)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif r<=4 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8600008, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 291)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8600008)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif r<=6 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8600009, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 291)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8600009)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif r<=8 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8600010, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 291)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8600010)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif r<=10 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8600011, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 291)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8600011)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif r<=12 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8600012, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 291)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8600012)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif r<=67 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310013, 1, 1, 8, 420)
	elseif r<=112 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310015, 1, 1, 8, 420)
	elseif r<=167 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310025, 1, 1, 8, 420)
	elseif r<=212 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310027, 1, 1, 8, 420)
	elseif r<=267 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310037, 1, 1, 8, 420)
	elseif r<=312 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310039, 1, 1, 8, 420)
	elseif r<=367 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310049, 1, 1, 8, 420)
	elseif r<=412 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310051, 1, 1, 8, 420)
	elseif r<=467 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310061, 1, 1, 8, 420)
	elseif r<=512 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310063, 1, 1, 8, 420)
	elseif r<=567 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310073, 1, 1, 8, 420)
	elseif r<=612 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310075, 1, 1, 8, 420)
	elseif r<=667 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310085, 1, 1, 8, 420)
	elseif r<=712 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310087, 1, 1, 8, 420)
	elseif r<=767 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310097, 1, 1, 8, 420)
	elseif r<=812 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310099, 1, 1, 8, 420)
	elseif r<=867 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310109, 1, 1, 8, 420)
	elseif r<=912 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310111, 1, 1, 8, 420)
	elseif r<=962 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310121, 1, 1, 8, 420)
	elseif r<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2310123, 1, 1, 8, 420)
	end

end

function I3303332_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303332, 1, "I3303332_GiftBag")
aux.RegisterItemEvent(3303332, 0, "I3303332_CanUseGiftBag")
