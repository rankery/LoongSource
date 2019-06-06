function I1330004_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1140001, 10, -1, 8, 420)

	local temp = TargetID%3

	local sex = role.GetRoleSex(TargetID)

	if sex == 0 then

		if temp==0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140031, 1, 1, 8, 420)
		elseif temp==1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140221, 1, 1, 8, 420)
		elseif temp==2 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140241, 1, 1, 8, 420)
		end

	else

		if temp==0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140021, 1, 1, 8, 420)
		elseif temp==1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140211, 1, 1, 8, 420)
		elseif temp==2 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140231, 1, 1, 8, 420)
		end
	end
end

function I1330004_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330004, 1, "I1330004_GiftBag")
aux.RegisterItemEvent(1330004, 0, "I1330004_CanUseGiftBag")
