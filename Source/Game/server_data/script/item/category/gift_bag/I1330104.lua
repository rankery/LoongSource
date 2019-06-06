function I1330104_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1160101, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1160102, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1160103, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1160104, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1160105, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1160106, 10, -1, 8, 420)
end

function I1330104_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330104, 1, "I1330104_GiftBag")
aux.RegisterItemEvent(1330104, 0, "I1330104_CanUseGiftBag")
