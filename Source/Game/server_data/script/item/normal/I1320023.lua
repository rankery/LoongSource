function I1320023_OnUse(MapID, InstanceID, TypeID, TargetID)
	local i = role.GetRoleAttValue(MapID, InstanceID, TargetID, 81)
	if i >= 50 then
	    role.ModRoleAttValue(MapID, InstanceID, TargetID, 81, -50)
	else
	    role.ModRoleAttValue(MapID, InstanceID, TargetID, 81, -i)
	end
end

function I1320023_CanUse(MapID, InstanceID, TypeID, TargetID)
    local i=role.GetRoleAttValue(MapID, InstanceID, TargetID, 81)
	if i == nil or i <= 0 then
	    return 32, false
	else
	    return 0, false
	end
end

aux.RegisterItemEvent(1320023, 1, "I1320023_OnUse")
aux.RegisterItemEvent(1320023, 0, "I1320023_CanUse")
