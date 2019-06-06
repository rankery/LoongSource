function ZhiYeXiangLian_Use(MapID, InstanceID, TypeID, TargetID)
    local ItemID = ( TypeID - 1361011 ) * 100 + 8520003
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, ItemID, 1, -1, 8, 420)
end

function ZhiYeXiangLian_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1361011, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361011, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361012, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361012, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361013, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361013, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361014, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361014, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361015, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361015, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361016, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361016, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361017, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361017, 0, "ZhiYeXiangLian_CanUse")
aux.RegisterItemEvent(1361018, 1, "ZhiYeXiangLian_Use")
aux.RegisterItemEvent(1361018, 0, "ZhiYeXiangLian_CanUse")
