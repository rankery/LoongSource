

-- 怪物进入战斗触发示警
function c1006729_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为3个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1006729, 2, "c1006729_OnEnterCombat")

--需求8  击杀 1006729 后，给 1006720 添加防御增强buff(2017601)防御加50%


function c1006729_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--unit.AddBuff(MapID, InstanceID, 1006720, 2017601, 1006720)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

end
--注册
aux.RegisterCreatureEvent(1006729, 4, "c1006729_OnDie")
