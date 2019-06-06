function YuLingShiDai_OnUse(MapID, InstanceID, TypeID, TargetID)
    if TypeID <= 2330010 then
		--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
		role.AddRoleItem(MapID, InstanceID, TargetID, TypeID + 1, 1, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2320021, 2, -1, 8, 420)
	elseif TypeID <= 2330019 then
		--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
		role.AddRoleItem(MapID, InstanceID, TargetID, TypeID + 1, 1, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2320021, 5, -1, 8, 420)
	elseif TypeID == 2330020 then
		--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
		role.AddRoleItem(MapID, InstanceID, TargetID, 2320021, 5, -1, 8, 420)
	end
end

function YuLingShiDai_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end

	return 0, false
end

aux.RegisterItemEvent(2330001, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330002, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330003, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330004, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330005, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330006, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330007, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330008, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330009, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330010, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330011, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330012, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330013, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330014, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330015, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330016, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330017, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330018, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330019, 1, "YuLingShiDai_OnUse")
aux.RegisterItemEvent(2330020, 1, "YuLingShiDai_OnUse")

aux.RegisterItemEvent(2330001, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330002, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330003, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330004, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330005, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330006, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330007, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330008, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330009, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330010, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330011, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330012, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330013, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330014, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330015, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330016, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330017, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330018, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330019, 0, "YuLingShiDai_CanUse")
aux.RegisterItemEvent(2330020, 0, "YuLingShiDai_CanUse")

