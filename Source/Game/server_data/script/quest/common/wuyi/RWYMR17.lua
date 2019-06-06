
--»÷É±Ìì¾øÐ«ÂÑ[1007047],Ë¢ÐÂ5¸öÎ´³ÉÊìµÄ¶¾Ð«[1007048]

function c1007047_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1007048, x+2, y, z+2)
	map.MapCreateCreature(MapID, InstanceID, 1007048, x-2, y, z+2)
	map.MapCreateCreature(MapID, InstanceID, 1007048, x+2, y, z-2)
	map.MapCreateCreature(MapID, InstanceID, 1007048, x+2, y, z-2)
	map.MapCreateCreature(MapID, InstanceID, 1007048, x, y, z+2)

end
aux.RegisterCreatureEvent(1007047,4,"c1007047_OnDie")
