--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009036_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 50010)

end

aux.RegisterCreatureEvent(1009036, 2, "c1009036_OnEnterCombat")


