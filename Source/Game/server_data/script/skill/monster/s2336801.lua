
--使用技能召唤怪物脚本

--穷奇（1290111）使用技能 （2336801） ，召唤3只怪物(1290122)


function s2336801_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1290122, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1290122, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1290122, x, y, z)
	return 0
end


--注册

aux.RegisterSkillEvent(2336801, 1, "s2336801_Cast")
