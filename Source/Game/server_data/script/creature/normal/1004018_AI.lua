
--怪物死亡时函数
--[[function c1004018_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateColCreature(MapID, InstanceID, 3020032, x, y, z, 1, "c01")
end]]

--注册函数
--aux.RegisterCreatureEvent(1004018, 4, "c1004018_OnDie")
