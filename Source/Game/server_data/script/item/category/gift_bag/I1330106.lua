function I1330106_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410011, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410023, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410035, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410047, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410059, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410071, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410083, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1410087, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1240011, 1, -1, 8, 420)
	local sex = role.GetRoleSex(TargetID)
	if sex == 0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9130021, 1, 1, 8, 420)
	elseif sex == 1 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9130031, 1, 1, 8, 420)
	end
end

function I1330106_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 10) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330106, 1, "I1330106_GiftBag")
aux.RegisterItemEvent(1330106, 0, "I1330106_CanUseGiftBag")
