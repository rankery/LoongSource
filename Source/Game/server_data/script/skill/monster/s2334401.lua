
--使用技能召唤怪物脚本

--魍魉 （1290101） 使用技能 2334401  召唤怪物(1003134,1003142)


function s2334401_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y, z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1003134, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1003142, x, y, z)
	return 0
end


--注册

aux.RegisterSkillEvent(2334401, 1, "s2334401_Cast")
