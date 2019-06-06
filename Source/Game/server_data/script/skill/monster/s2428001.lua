--2428001 ÎÊ¿Í

function s2428001_Cast(MapID, InstanceID, SkillID, OwnerID)
	local k = math.random(1,10)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	if k >= 1 and k <= 7 then
	map.MapCreateCreature(MapID, InstanceID, 1011014, x-4, y, z-4)
	else
	map.MapCreateCreature(MapID, InstanceID, 1011014, x-4, y, z-4)
	map.MapCreateCreature(MapID, InstanceID, 1011014, x-4, y, z-4)
	end
	return 0
end

--×¢²á

aux.RegisterSkillEvent(2428001, 1, "s2428001_Cast")

















