function I1330021_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1000311, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1000316, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4008005, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4009003, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1360133, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1360123, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1340001, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200464, 50, -1, 8, 420)
end

function I1330021_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 8) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330021, 1, "I1330021_GiftBag")
aux.RegisterItemEvent(1330021, 0, "I1330021_CanUseGiftBag")
