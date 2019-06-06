--可否使用函数
function I1330020_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330020, 0, "I1330020_CanUse")

function I1330020_OnUse(MapID, InstanceID, TypeID, TargetID)
	local r = math.random(100)
	if r >84 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2220401, 1, -1, 8, 102)
	elseif r >37 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3300103, 10, -1, 8, 102)
	elseif r > 0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1320023, 1, -1, 8, 102)
	end
end

aux.RegisterItemEvent(1330020, 1, "I1330020_OnUse")
--点金石(20%)，仙界天劫令*10(45%)，副本疲劳值道具（35%）
