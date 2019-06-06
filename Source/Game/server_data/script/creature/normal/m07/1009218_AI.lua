--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009218_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50028)

end

aux.RegisterCreatureEvent(1009218, 2, "c1009218_OnEnterCombat")



