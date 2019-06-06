function I3200484_OnUse(MapID, InstanceID, TypeID, TargetID)
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateColCreature(MapID, InstanceID, 1543901, x, y, z, 1)
end

function I3200484_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID == 2291722097 or MapID == 2291721329 or MapID == 2291721585 or MapID == 2291720817 or MapID == 2291721073 or MapID == 2291720305 or MapID == 2291720561 then
		return bRet, bIgnore
	else
		bRet = 43
	    return bRet, bIgnore
	end
end

aux.RegisterItemEvent(3200484, 1, "I3200484_OnUse")
aux.RegisterItemEvent(3200484, 0, "I3200484_CanUse")
