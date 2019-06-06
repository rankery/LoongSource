
-- 怪物进入战斗触发示警
function c1008503_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008503, 2, "c1008503_OnEnterCombat")


--击杀赤魅娘娘(typeID：1008503)后，在(m18,620,22049,592)位置刷新typeID为1008506的碰撞怪物，巡逻路线为B02；
function c1008503_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1008506, 620, 22049, 592, 1, "B02")
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008503, 4, "c1008503_OnDie")
