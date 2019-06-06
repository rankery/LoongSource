--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006138_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30131)
	
end

aux.RegisterCreatureEvent(1006138, 2, "c1006138_OnEnterCombat")

