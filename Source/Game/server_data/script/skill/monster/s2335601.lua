
--使用技能召唤怪物脚本

--混沌（1290106）使用技能 （23345601） ，召唤怪物(1290116,1290117)


function s2335601_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1290116, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1290117, x, y, z)
	return 0
end


--注册

aux.RegisterSkillEvent(2335601, 1, "s2335601_Cast")
