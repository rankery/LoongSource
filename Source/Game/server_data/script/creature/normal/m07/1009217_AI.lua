--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009217_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50027)

end

aux.RegisterCreatureEvent(1009217, 2, "c1009217_OnEnterCombat")




