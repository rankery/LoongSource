--ÁøÖ¦
function I3303710_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)

	if MapID == 3017298383 and math.abs(x-2462)<100 and math.abs(z-1488)<100 then
		return 0,false
	elseif MapID == 3017299663 and math.abs(x-1790)<100 and math.abs(z-1886)<100 then
		return 0,false
	elseif MapID == 3017299151 and math.abs(x-1809)<100 and math.abs(z-1819)<100 then
		return 0,false
	else
		return 43, false
	end
end

aux.RegisterItemEvent(3303710, 0, "I3303710_CanUse")
