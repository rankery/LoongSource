--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005509_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20101)

end

aux.RegisterCreatureEvent(1005509, 2, "c1005509_OnEnterCombat")

