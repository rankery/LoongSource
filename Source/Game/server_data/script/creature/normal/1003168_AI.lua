--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003168_OnEnterCombat(MapID, InstanceID, CreatureID)
       
       cre.MonsterSay(MapID, InstanceID, CreatureID, 43)
       	
end

aux.RegisterCreatureEvent(1003168, 2, "c1003168_OnEnterCombat")


