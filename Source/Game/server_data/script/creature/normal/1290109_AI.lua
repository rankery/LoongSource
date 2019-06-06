--怪物喊话脚本

-- 怪物进入战斗喊话
function c1290109_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 20110)
end

aux.RegisterCreatureEvent(1290109, 2, "c1290109_OnEnterCombat")



