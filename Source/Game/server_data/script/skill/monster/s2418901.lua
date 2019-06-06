
----使用技能召唤怪物脚本

--邪心法王（1010051）使用技能 （2418901） ，召唤4个怪物1010052

function s2418901_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1010052, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1010052, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1010052, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1010052, x, y, z)
	return 0
end


--注册

aux.RegisterSkillEvent(2418901, 1, "s2418901_Cast")
