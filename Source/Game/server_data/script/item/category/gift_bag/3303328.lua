function I3303328_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140611, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140612, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140613, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140614, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140615, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140616, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140617, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140618, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140619, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140620, 1, 1, 8, 420)

	end
end

function I3303328_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303328, 1, "I3303328_GiftBag")
aux.RegisterItemEvent(3303328, 0, "I3303328_CanUseGiftBag")

function I3303329_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140601, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140602, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140603, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140604, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140605, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140606, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140607, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140608, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140609, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140610, 1, 1, 8, 420)

	end
end

function I3303329_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303329, 1, "I3303329_GiftBag")
aux.RegisterItemEvent(3303329, 0, "I3303329_CanUseGiftBag")

function I3303330_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140631, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140632, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140633, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140634, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140635, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140636, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140637, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140638, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140639, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140640, 1, 1, 8, 420)

	end
end

function I3303330_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303330, 1, "I3303330_GiftBag")
aux.RegisterItemEvent(3303330, 0, "I3303330_CanUseGiftBag")

function I3303331_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140621, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140622, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140623, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140624, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140625, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140626, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140627, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140628, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140629, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140630, 1, 1, 8, 420)

	end
end

function I3303331_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303331, 1, "I3303331_GiftBag")
aux.RegisterItemEvent(3303331, 0, "I3303331_CanUseGiftBag")

function I3303333_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140641, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140642, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140643, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140644, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140645, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140646, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140647, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140648, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140649, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140650, 1, 1, 8, 420)

	end
end

function I3303333_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303333, 1, "I3303333_GiftBag")
aux.RegisterItemEvent(3303333, 0, "I3303333_CanUseGiftBag")

function I3303334_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local sex = role.GetRoleSex(TargetID)

	if sex == 1 then

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140651, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140652, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140653, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140654, 1, 1, 8, 420)

	else

		role.AddRoleItem(MapID, InstanceID, TargetID, 9140655, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140656, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140657, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140658, 1, 1, 8, 420)

	end
end

function I3303334_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303334, 1, "I3303334_GiftBag")
aux.RegisterItemEvent(3303334, 0, "I3303334_CanUseGiftBag")
