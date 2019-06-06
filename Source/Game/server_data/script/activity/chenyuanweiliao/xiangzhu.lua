--ƒ˝À™÷Æœ„÷Ú
function I2611601_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if role.GetBagFreeSize(TargetID) == 0 then
		return 40, bIgnore
	end

	if MapID ~= 3017298383 then
		return 43, bIgnore
	end

	if x<820 then
		return 43, bIgnore
	end
	if x>852 then
		return 43, bIgnore
	end
	if z<1019 then
		return 43, bIgnore
	end
	if z>1051 then
		return 43, bIgnore
	end

	return bRet, bIgnore
end

function I2611601_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2611606, 1, -1, 8, 420)
end

--≥’—©÷Æœ„÷Ú
function I2611602_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if role.GetBagFreeSize(TargetID) == 0 then
		return 40, bIgnore
	end

	if MapID ~= 3017298383 then
		return 43, bIgnore
	end

	if x<478 then
		return 43, bIgnore
	end
	if x>510 then
		return 43, bIgnore
	end
	if z<818 then
		return 43, bIgnore
	end
	if z>850 then
		return 43, bIgnore
	end

	return bRet, bIgnore
end

function I2611602_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2611607, 1, -1, 8, 420)
end

--¡µ—˛÷Æœ„÷Ú
function I2611603_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if role.GetBagFreeSize(TargetID) == 0 then
		return 40, bIgnore
	end

	if MapID ~= 3017298383 then
		return 43, bIgnore
	end

	if x<2158 then
		return 43, bIgnore
	end
	if x>2190 then
		return 43, bIgnore
	end
	if z<2593 then
		return 43, bIgnore
	end
	if z>2625 then
		return 43, bIgnore
	end

	return bRet, bIgnore
end

function I2611603_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2611608, 1, -1, 8, 420)
end

--”Ô÷Ò÷Æœ„÷Ú
function I2611604_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if role.GetBagFreeSize(TargetID) == 0 then
		return 40, bIgnore
	end

	if MapID ~= 3017298383 then
		return 43, bIgnore
	end

	if x<2414 then
		return 43, bIgnore
	end
	if x>2446 then
		return 43, bIgnore
	end
	if z<1551 then
		return 43, bIgnore
	end
	if z>1583 then
		return 43, bIgnore
	end

	return bRet, bIgnore
end

function I2611604_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2611609, 1, -1, 8, 420)
end

--∫¨»Ù÷Æœ„÷Ú
function I2611605_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if role.GetBagFreeSize(TargetID) == 0 then
		return 40, bIgnore
	end

	if MapID ~= 3017298383 then
		return 43, bIgnore
	end

	if x<2017 then
		return 43, bIgnore
	end
	if x>2049 then
		return 43, bIgnore
	end
	if z<851 then
		return 43, bIgnore
	end
	if z>883 then
		return 43, bIgnore
	end

	return bRet, bIgnore
end

function I2611605_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2611610, 1, -1, 8, 420)
end

aux.RegisterItemEvent(2611601, 1, "I2611601_QuestUsable")
aux.RegisterItemEvent(2611602, 1, "I2611602_QuestUsable")
aux.RegisterItemEvent(2611603, 1, "I2611603_QuestUsable")
aux.RegisterItemEvent(2611604, 1, "I2611604_QuestUsable")
aux.RegisterItemEvent(2611605, 1, "I2611605_QuestUsable")
aux.RegisterItemEvent(2611601, 0, "I2611601_CanUse")
aux.RegisterItemEvent(2611602, 0, "I2611602_CanUse")
aux.RegisterItemEvent(2611603, 0, "I2611603_CanUse")
aux.RegisterItemEvent(2611604, 0, "I2611604_CanUse")
aux.RegisterItemEvent(2611605, 0, "I2611605_CanUse")
