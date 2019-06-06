
--神炼矿石
function i3304002_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

    local i = role.GetRoleItemNum(TargetID, 3304002)
	if i < 10 then
        bRet = 32
	end

	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end

	--返回
	return bRet, bIgnore

end

--使用效果
function i3304002_OnUse(MapID, InstanceID, TypeID, TargetID)

    role.RemoveFromRole(MapID, InstanceID, TargetID, 3304002, 9, 420)
    role.AddRoleItem(MapID, InstanceID, TargetID, 3304003, 1, -1, 8, 420)

end

--注册
aux.RegisterItemEvent(3304002, 1, "i3304002_OnUse")
aux.RegisterItemEvent(3304002, 0, "i3304002_CanUse")

