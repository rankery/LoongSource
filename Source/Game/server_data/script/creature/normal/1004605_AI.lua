--怪物喊话脚本

-- 怪物进入战斗喊话/示警
function c1004605_OnEnterCombat(MapID, InstanceID, CreatureID)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为70个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 70, 20)
    cre.MonsterSay(MapID, InstanceID, CreatureID, 10052)

end

aux.RegisterCreatureEvent(1004605, 2, "c1004605_OnEnterCombat")


--怪物死亡喊话
function c1004605_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 10053)

end

aux.RegisterCreatureEvent(1004605, 4, "c1004605_OnDie")

