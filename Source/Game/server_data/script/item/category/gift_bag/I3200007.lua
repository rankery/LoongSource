function I3200007_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)

	rolelevel = math.floor(rolelevel/30)+1


	role.AddRoleItem(MapID, InstanceID, TargetID, 1140001, rolelevel, -1, 8, 420)

end

function I3200007_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200007, 1, "I3200007_GiftBag")
aux.RegisterItemEvent(3200007, 0, "I3200007_CanUseGiftBag")
