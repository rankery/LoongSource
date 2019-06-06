function I1330101_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330301, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330302, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800019, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1300102, 2, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1340001, 2, -1, 8, 420)
end

function I1330101_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330101, 1, "I1330101_GiftBag")
aux.RegisterItemEvent(1330101, 0, "I1330101_CanUseGiftBag")
