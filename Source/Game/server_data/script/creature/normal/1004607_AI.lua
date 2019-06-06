--怪物喊话脚本

-- 怪物进入战斗喊话
function c1004607_OnEnterCombat(MapID, InstanceID, CreatureID)
       
             cre.MonsterSay(MapID, InstanceID, CreatureID, 10057)

end

aux.RegisterCreatureEvent(1004607, 2, "c1004607_OnEnterCombat")

