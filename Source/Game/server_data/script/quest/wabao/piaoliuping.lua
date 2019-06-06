function I1350041_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举

	local jiyuid = 1350200
	role.AddRoleItem(MapID, InstanceID, TargetID, jiyuid+math.random(11), 1, -1, 8, 420)


end

function I1350041_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1350041, 1, "I1350041_GiftBag")
aux.RegisterItemEvent(1350041, 0, "I1350041_CanUseGiftBag")
