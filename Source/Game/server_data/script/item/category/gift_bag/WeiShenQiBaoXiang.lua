function I3305101_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local r = math.random(100)

	if r<=15 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1320023, 4, -1, 8, 420)

	elseif r<=25 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360007, 1, -1, 8, 420)

	elseif r<=35 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360008, 1, -1, 8, 420)

	elseif r<=45 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005203, 3, -1, 8, 420)

	elseif r<=65 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3303026, 5, -1, 8, 420)

	elseif r<=85 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360124, 4, -1, 8, 420)

	elseif r<=99 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005303, 4, -1, 8, 420)

	elseif r<=100 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907101+TypeID-3305101, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 292)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 8907101+TypeID-3305101)
		msg.DispatchWorldMsgEvent(MsgID)
	end

end

function I3305101_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3305101, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305102, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305103, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305104, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305105, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305106, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305107, 1, "I3305101_GiftBag")
aux.RegisterItemEvent(3305108, 1, "I3305101_GiftBag")

aux.RegisterItemEvent(3305101, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305102, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305103, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305104, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305105, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305106, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305107, 0, "I3305101_CanUseGiftBag")
aux.RegisterItemEvent(3305108, 0, "I3305101_CanUseGiftBag")
