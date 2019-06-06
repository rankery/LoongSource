--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003065_OnEnterCombat(MapID, InstanceID, CreatureID)
       
             cre.MonsterSay(MapID, InstanceID, CreatureID, 1)	
end

aux.RegisterCreatureEvent(1003065, 2, "c1003065_OnEnterCombat")

