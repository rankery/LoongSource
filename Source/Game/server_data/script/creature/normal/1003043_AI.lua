--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003043_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 12)

end

aux.RegisterCreatureEvent(1003043, 2, "c1003043_OnEnterCombat")

