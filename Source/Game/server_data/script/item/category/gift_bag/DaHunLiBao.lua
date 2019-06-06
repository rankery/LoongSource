function Marriage_GiftBag(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140081, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140082, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140083, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140084, 1, 1, 8, 420)
	elseif sex == 0 then
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140091, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140092, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140093, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140094, 1, 1, 8, 420)
	end
end

function Marriage_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1361111, 1, "Marriage_GiftBag")
aux.RegisterItemEvent(1361111, 0, "Marriage_CanUseGiftBag")
aux.RegisterItemEvent(1361112, 1, "Marriage_GiftBag")
aux.RegisterItemEvent(1361112, 0, "Marriage_CanUseGiftBag")


