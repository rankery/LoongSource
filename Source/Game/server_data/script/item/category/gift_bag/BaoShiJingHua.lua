function BaoShiJingHua(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200707, 1, -1, 8, 420)
end

function BaoShiJingHua_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200701, 1, "BaoShiJingHua")
aux.RegisterItemEvent(3200701, 0, "BaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200702, 1, "BaoShiJingHua")
aux.RegisterItemEvent(3200702, 0, "BaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200703, 1, "BaoShiJingHua")
aux.RegisterItemEvent(3200703, 0, "BaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200704, 1, "BaoShiJingHua")
aux.RegisterItemEvent(3200704, 0, "BaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200705, 1, "BaoShiJingHua")
aux.RegisterItemEvent(3200705, 0, "BaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200706, 1, "BaoShiJingHua")
aux.RegisterItemEvent(3200706, 0, "BaoShiJingHua_CanUse")
