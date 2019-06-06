--怪物喊话脚本

-- 怪物进入战斗喊话
function x1005312_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20040)

end

aux.RegisterCreatureEvent(1005312, 2, "x1005312_OnEnterCombat")

--怪物死亡喊话
function x1005312_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 20042)

end

aux.RegisterCreatureEvent(1005312, 4, "x1005312_OnDie")

