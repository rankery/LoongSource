--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005123_OnEnterCombat(MapID, InstanceID, CreatureID)
       
             cre.MonsterSay(MapID, InstanceID, CreatureID, 20010)
	
end

aux.RegisterCreatureEvent(1005123, 2, "c1005123_OnEnterCombat")

