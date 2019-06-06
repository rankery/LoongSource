
function c1007175_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
		if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 14449) then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 3050153, x+2, y, z+2)
		end
end
aux.RegisterCreatureEvent(1007175,4,"c1007175_OnDie")

