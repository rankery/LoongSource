--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010107_OnEnterCombat(MapID, InstanceID, CreatureID)

	      cre.MonsterSay(MapID, InstanceID, CreatureID, 50052)

end

aux.RegisterCreatureEvent(1010107, 2, "c1010107_OnEnterCombat")




