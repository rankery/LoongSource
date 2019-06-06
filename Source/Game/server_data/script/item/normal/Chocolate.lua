function chocolate_OnUse(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local k = math.random (100)
	if k <= 8 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303008, 1, -1, 8, 420)
	end
end



aux.RegisterItemEvent(3303134, 1, "chocolate_OnUse")
aux.RegisterItemEvent(3303135, 1, "chocolate_OnUse")

