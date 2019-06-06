--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005503_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20097)
	
end

aux.RegisterCreatureEvent(1005503, 2, "c1005503_OnEnterCombat")

