--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006321_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30040)

end

aux.RegisterCreatureEvent(1006321, 2, "c1006321_OnEnterCombat")

