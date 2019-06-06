function I1330203_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330307, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330308, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800015, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800016, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800017, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800018, 1, 3, 8, 420)
end

function I1330203_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330203, 1, "I1330203_GiftBag")
aux.RegisterItemEvent(1330203, 0, "I1330203_CanUseGiftBag")
