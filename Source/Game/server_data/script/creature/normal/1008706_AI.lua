--怪物喊话脚本

-- 怪物进入战斗喊话
function c1008706_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 40079)

end

aux.RegisterCreatureEvent(1008706, 2, "c1008706_OnEnterCombat")


--怪物死亡喊话
function c1008706_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 40080)

end

aux.RegisterCreatureEvent(1008706, 4, "c1008706_OnDie")

