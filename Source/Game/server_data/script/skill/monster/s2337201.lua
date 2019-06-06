
--使用技能召唤怪物脚本

--彭侯（1290112）使用技能 （2337201） ，召唤1个怪物1290120，3个怪物1290121


function s2337201_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1290120, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1290121, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1290121, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1290121, x, y, z)
	return 0
end


--注册

aux.RegisterSkillEvent(2337201, 1, "s2337201_Cast")
