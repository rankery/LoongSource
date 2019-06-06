--·杀死 枭吴魔将 1008001 刷出 传送石 3060027 / 播放特效 txcj09 坐标：x 1464 y 15330 z 598
function c1008001_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3060027, 1464, 15330, 598)
end
aux.RegisterCreatureEvent(1008001, 4, "c1008001_OnDie")

-- 怪物进入战斗触发示警
function c1008001_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008001, 2, "c1008001_OnEnterCombat")
