--·击杀 贪金近侍 1008078 刷出 贪金 1008074 坐标：x 2819 y 26387 z 684 ，场景内所有 鬼叉骠骑 1008073 消失
function c1008078_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008074, 2819, 26387, 684)
	--map.MapDeleteCreature(MapID, InstanceID, 1008073)

end
aux.RegisterCreatureEvent(1008078, 4, "c1008078_OnDie")

-- 怪物进入战斗触发示警
function c1008078_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008078, 2, "c1008078_OnEnterCombat")
