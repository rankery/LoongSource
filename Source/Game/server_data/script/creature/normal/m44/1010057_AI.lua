--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010057_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50045)

end

aux.RegisterCreatureEvent(1010057, 2, "c1010057_OnEnterCombat")



