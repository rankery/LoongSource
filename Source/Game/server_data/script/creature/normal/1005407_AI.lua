--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005407_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20051)

end

aux.RegisterCreatureEvent(1005407, 2, "c1005407_OnEnterCombat")

--怪物死亡喊话
function c1005407_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 20053)
end

aux.RegisterCreatureEvent(1005407, 4, "c1005407_OnDie")

