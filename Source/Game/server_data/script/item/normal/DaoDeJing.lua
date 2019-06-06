
--道德经卷一
function I3303707_OnUse(MapID, InstanceID, TypeID, TargetID)
	local Mor = role.GetRoleAttValue(MapID, InstanceID, TargetID, 74)
	if Mor <= 45 then
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 74, 5)
	end
end

function I3303707_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Mor = role.GetRoleAttValue(MapID, InstanceID, RoleID, 74)
	if Mor > 45 then
		return 32,false
	end
end

aux.RegisterItemEvent(3303707, 1, "I3303707_OnUse")
aux.RegisterItemEvent(3303707, 0, "I3303707_CanUse")

--道德经卷二
function I3303708_OnUse(MapID, InstanceID, TypeID, TargetID)
	local Mor = role.GetRoleAttValue(MapID, InstanceID, TargetID, 74)
	if Mor <= 45 then
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 74, 10)
	end
end
function I3303708_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Mor = role.GetRoleAttValue(MapID, InstanceID, RoleID, 74)
	if Mor > 45 then
		return 32,false
	end
end

aux.RegisterItemEvent(3303708, 1, "I3303708_OnUse")
aux.RegisterItemEvent(3303708, 0, "I3303708_CanUse")
