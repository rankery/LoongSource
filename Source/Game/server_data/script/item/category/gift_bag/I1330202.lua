function I1330202_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1421008, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1421012, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1421016, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1421020, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1421032, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1421036, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1230016, 6, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1300002, 1, -1, 8, 420)
end

function I1330202_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 8) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330202, 1, "I1330202_GiftBag")
aux.RegisterItemEvent(1330202, 0, "I1330202_CanUseGiftBag")
