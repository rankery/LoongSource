
-- 怪物进入战斗触发示警
function c1008046_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008046, 2, "c1008046_OnEnterCombat")

--·击杀 腐炽魔帅 1008046 刷出 刑金魔帅 1008047 坐标：x 1902 y 21000 z 670
function c1008046_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008047, 1902, 21000, 670)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008046, 4, "c1008046_OnDie")
