--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006613_OnEnterCombat(MapID, InstanceID, CreatureID)
          
          cre.MonsterSay(MapID, InstanceID, CreatureID, 30112)
end

aux.RegisterCreatureEvent(1006613, 2, "c1006613_OnEnterCombat")

