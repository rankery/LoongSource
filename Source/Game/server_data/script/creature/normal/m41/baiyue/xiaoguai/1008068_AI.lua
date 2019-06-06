--怪物鬼叉斥候脚本——ID：1008068
--死亡时 原地刷新出怪物伏云亲兵1008012

--死亡时脚本
function c1008068_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	map.MapCreateCreature(MapID, InstanceID, 1008012, x, y+1, z)

end
--注册
aux.RegisterCreatureEvent(1008068, 4, "c1008068_OnDie")
