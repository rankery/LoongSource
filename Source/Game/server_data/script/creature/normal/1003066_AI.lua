--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003066_OnEnterCombat(MapID, InstanceID, CreatureID)

         cre.MonsterSay(MapID, InstanceID, CreatureID, 2)
       	
end

aux.RegisterCreatureEvent(1003066, 2, "c1003066_OnEnterCombat")

