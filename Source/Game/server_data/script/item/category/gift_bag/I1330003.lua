function I1330003_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 2610163, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2610164, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330004, 1, -1, 8, 420)

	local temp = TargetID%3

	local sex = role.GetRoleSex(TargetID)

	if sex == 0 then

		if temp==0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140034, 1, 1, 8, 420)
		elseif temp==1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140224, 1, 1, 8, 420)
		elseif temp==2 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140244, 1, 1, 8, 420)
		end

	else

		if temp==0 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140024, 1, 1, 8, 420)
		elseif temp==1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140214, 1, 1, 8, 420)
		elseif temp==2 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 9140234, 1, 1, 8, 420)
		end
	end
end

function I1330003_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330003, 1, "I1330003_GiftBag")
aux.RegisterItemEvent(1330003, 0, "I1330003_CanUseGiftBag")
