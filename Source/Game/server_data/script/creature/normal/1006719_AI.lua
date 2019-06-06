-- 怪物进入战斗触发示警
function c1006719_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为3个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1006719, 2, "c1006719_OnEnterCombat")

--需求3  击杀 1006719 后，在坐标 x=972 y= 2680 z=763 刷出 通冥灵石 3040534

function c1006719_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3040534, 972, 2680, 763)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

end
--注册
aux.RegisterCreatureEvent(1006719,4,"c1006719_OnDie")


