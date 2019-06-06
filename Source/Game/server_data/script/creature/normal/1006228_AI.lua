--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006228_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30029)

end

aux.RegisterCreatureEvent(1006228, 2, "c1006228_OnEnterCombat")

