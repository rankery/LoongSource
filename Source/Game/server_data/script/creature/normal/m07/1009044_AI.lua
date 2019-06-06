--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009044_OnEnterCombat(MapID, InstanceID, CreatureID)

	 cre.MonsterSay(MapID, InstanceID, CreatureID, 50012)

end

aux.RegisterCreatureEvent(1009044, 2, "c1009044_OnEnterCombat")


