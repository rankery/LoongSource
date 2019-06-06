--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009065_OnEnterCombat(MapID, InstanceID, CreatureID)

	      cre.MonsterSay(MapID, InstanceID, CreatureID, 50017)

end

aux.RegisterCreatureEvent(1009065, 2, "c1009065_OnEnterCombat")


