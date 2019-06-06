--怪物鬼叉斥候脚本——ID：1008068
--死亡时 原地刷新出怪物伏云亲兵1008012

--死亡时脚本
function c1008068_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	map.MapCreateCreature(MapID, InstanceID, 1008012, x, y+1, z)

end
--注册
aux.RegisterCreatureEvent(1008068, 4, "c1008068_OnDie")


-- 怪物进入战斗触发示警
function c1008068_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008068, 2, "c1008068_OnEnterCombat")
