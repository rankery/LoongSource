function I3303324_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140511, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140512, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140513, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140514, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140515, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140516, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140517, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140518, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140519, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140520, 1, 1, 8, 420)

	end
end

function I3303324_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303324, 1, "I3303324_GiftBag")
aux.RegisterItemEvent(3303324, 0, "I3303324_CanUseGiftBag")

function I3303325_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140501, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140502, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140503, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140504, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140505, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140506, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140507, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140508, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140509, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140510, 1, 1, 8, 420)

	end
end

function I3303325_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303325, 1, "I3303325_GiftBag")
aux.RegisterItemEvent(3303325, 0, "I3303325_CanUseGiftBag")

function I3303326_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140531, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140532, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140533, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140534, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140535, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140536, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140537, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140538, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140539, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140540, 1, 1, 8, 420)

	end
end

function I3303326_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303326, 1, "I3303326_GiftBag")
aux.RegisterItemEvent(3303326, 0, "I3303326_CanUseGiftBag")

function I3303327_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140521, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140522, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140523, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140524, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140525, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140526, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140527, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140528, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140529, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140530, 1, 1, 8, 420)

	end
end

function I3303327_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303327, 1, "I3303327_GiftBag")
aux.RegisterItemEvent(3303327, 0, "I3303327_CanUseGiftBag")
