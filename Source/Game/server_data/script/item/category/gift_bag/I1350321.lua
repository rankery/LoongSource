function I1350321_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local rolesex = role.GetRoleSex(TargetID)
	if rolesex == 0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900012, 1, 2, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900013, 1, 2, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900014, 1, 2, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900015, 1, 2, 8, 420)
	elseif rolesex == 1 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900002, 1, 2, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900003, 1, 2, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900004, 1, 2, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9900005, 1, 2, 8, 420)
	end
end

function I11350321_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1350321, 1, "I1350321_GiftBag")
aux.RegisterItemEvent(1350321, 0, "I1350321_CanUseGiftBag")
