--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005510_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20102)
	
end

aux.RegisterCreatureEvent(1005510, 2, "c1005510_OnEnterCombat")

