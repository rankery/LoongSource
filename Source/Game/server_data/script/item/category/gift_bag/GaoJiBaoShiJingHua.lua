function GaoJiBaoShiJingHua(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200727, 1, -1, 8, 420)
end

function GaoJiBaoShiJingHua_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200721, 1, "GaoJiBaoShiJingHua")
aux.RegisterItemEvent(3200721, 0, "GaoJiBaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200722, 1, "GaoJiBaoShiJingHua")
aux.RegisterItemEvent(3200722, 0, "GaoJiBaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200723, 1, "GaoJiBaoShiJingHua")
aux.RegisterItemEvent(3200723, 0, "GaoJiBaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200724, 1, "GaoJiBaoShiJingHua")
aux.RegisterItemEvent(3200724, 0, "GaoJiBaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200725, 1, "GaoJiBaoShiJingHua")
aux.RegisterItemEvent(3200725, 0, "GaoJiBaoShiJingHua_CanUse")

aux.RegisterItemEvent(3200726, 1, "GaoJiBaoShiJingHua")
aux.RegisterItemEvent(3200726, 0, "GaoJiBaoShiJingHua_CanUse")
