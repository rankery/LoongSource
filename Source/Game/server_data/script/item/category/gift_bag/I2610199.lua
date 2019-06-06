function I2610199_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 2610012, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8900011, 1, -1, 8, 420)

end

function I2610199_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet, bIgnore = 40, false
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(2610199, 1, "I2610199_GiftBag")
aux.RegisterItemEvent(2610199, 0, "I2610199_CanUseGiftBag")
