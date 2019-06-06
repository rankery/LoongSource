--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005511_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20104)
	
end

aux.RegisterCreatureEvent(1005511, 2, "c1005511_OnEnterCombat")

