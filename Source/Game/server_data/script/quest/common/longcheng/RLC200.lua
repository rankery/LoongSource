--使用道具太昭原石[2610195]获得道具太昭灵石[2610196]

function I2610195_OnUse(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 2610196, 1, -1, 8, 420)
end

function I2610195_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(2610195, 1, "I2610195_OnUse")
aux.RegisterItemEvent(2610195, 0, "I2610195_CanUse")