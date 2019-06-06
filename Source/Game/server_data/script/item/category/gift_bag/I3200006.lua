function I3200006_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)

	rolelevel = math.floor(rolelevel/10)+1


	role.AddRoleItem(MapID, InstanceID, TargetID, 1000900+rolelevel, 5, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1000506, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1210001, 1, -1, 8, 420)
end

function I3200006_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200006, 1, "I3200006_GiftBag")
aux.RegisterItemEvent(3200006, 0, "I3200006_CanUseGiftBag")
