
----使用技能召唤怪物脚本

--邪毒咒王（1010004）使用技能 （2418701） ，召唤4个怪物1010011

function s2418701_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1010011, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1010011, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1010011, x, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1010011, x, y, z)
	return 0
end


--注册

aux.RegisterSkillEvent(2418701, 1, "s2418701_Cast")

