--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009059_OnEnterCombat(MapID, InstanceID, CreatureID)

	      cre.MonsterSay(MapID, InstanceID, CreatureID, 50016)

end

aux.RegisterCreatureEvent(1009059, 2, "c1009059_OnEnterCombat")


