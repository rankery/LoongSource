function I2000035_OnUse(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000026, 1, -1, 8, 420)
end
function I2000035_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false		--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then				--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000036_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000027, 1, -1, 8, 420)
end
function I2000036_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000037_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000028, 1, -1, 8, 420)
end
function I2000037_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000038_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000029, 1, -1, 8, 420)
end
function I2000038_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000039_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000030, 1, -1, 8, 420)
end
function I2000039_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000235_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000226, 1, -1, 8, 420)
end
function I2000235_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000236_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000227, 1, -1, 8, 420)
end
function I2000236_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000237_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000228, 1, -1, 8, 420)
end
function I2000237_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000238_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000229, 1, -1, 8, 420)
end
function I2000238_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

function I2000239_OnUse(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000230, 1, -1, 8, 420)
end
function I2000239_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(2000035, 1, "I2000035_OnUse")
aux.RegisterItemEvent(2000035, 0, "I2000035_CanUse")
aux.RegisterItemEvent(2000036, 1, "I2000036_OnUse")
aux.RegisterItemEvent(2000036, 0, "I2000036_CanUse")
aux.RegisterItemEvent(2000037, 1, "I2000037_OnUse")
aux.RegisterItemEvent(2000037, 0, "I2000037_CanUse")
aux.RegisterItemEvent(2000038, 1, "I2000038_OnUse")
aux.RegisterItemEvent(2000038, 0, "I2000038_CanUse")
aux.RegisterItemEvent(2000039, 1, "I2000039_OnUse")
aux.RegisterItemEvent(2000039, 0, "I2000039_CanUse")

aux.RegisterItemEvent(2000235, 1, "I2000235_OnUse")
aux.RegisterItemEvent(2000235, 0, "I2000235_CanUse")
aux.RegisterItemEvent(2000236, 1, "I2000236_OnUse")
aux.RegisterItemEvent(2000236, 0, "I2000236_CanUse")
aux.RegisterItemEvent(2000237, 1, "I2000237_OnUse")
aux.RegisterItemEvent(2000237, 0, "I2000237_CanUse")
aux.RegisterItemEvent(2000238, 1, "I2000238_OnUse")
aux.RegisterItemEvent(2000238, 0, "I2000238_CanUse")
aux.RegisterItemEvent(2000239, 1, "I2000239_OnUse")
aux.RegisterItemEvent(2000239, 0, "I2000239_CanUse")
