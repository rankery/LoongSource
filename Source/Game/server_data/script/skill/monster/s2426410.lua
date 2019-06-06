
----使用技能召唤怪物脚本

--鬼道幻使（1006522）使用技能 （2426410） ，召唤2个怪物1006536

function s2426410_Cast(MapID, InstanceID, SkillID, OwnerID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, OwnerID)
	map.MapCreateCreature(MapID, InstanceID, 1006536, x-4, y, z-4)
	map.MapCreateCreature(MapID, InstanceID, 1006536, x+4, y, z+4)
	return 0
end


--注册

aux.RegisterSkillEvent(2426410, 1, "s2426410_Cast")
