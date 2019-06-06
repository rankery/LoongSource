--·杀死 英灵亡魂 1008091 刷出 英灵宝箱 3060035 / 播放特效 txcj47 坐标：x 1154 y 14780 z 523
function c1008091_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3060035, 1154, 14780, 523)
end
aux.RegisterCreatureEvent(1008091, 4, "c1008091_OnDie")

-- 怪物进入战斗触发示警
function c1008091_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008091, 2, "c1008091_OnEnterCombat")
