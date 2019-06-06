function I3200461_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200463, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200464, 20, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200465, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200466, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200462, 1, -1, 8, 420)
end

function I3200461_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200461, 1, "I3200461_GiftBag")
aux.RegisterItemEvent(3200461, 0, "I3200461_CanUseGiftBag")

--礼品不能在修炼场外使用

function CanUseXinShouLiPin(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID == 2291722097 or MapID == 2291721329 or MapID == 2291721585 or MapID == 2291720817 or MapID == 2291721073 or MapID == 2291720305 or MapID == 2291720561 then
	    return bRet, bIgnore
	else
	    bRet = 43
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200465, 0, "CanUseXinShouLiPin")
aux.RegisterItemEvent(3200466, 0, "CanUseXinShouLiPin")
