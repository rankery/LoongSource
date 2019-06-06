--怪物喊话脚本

-- 怪物进入战斗喊话
function c1007116_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 40018)
	
end

aux.RegisterCreatureEvent(1007116, 2, "c1007116_OnEnterCombat")

