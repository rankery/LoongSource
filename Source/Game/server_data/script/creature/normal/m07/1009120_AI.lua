--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009120_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50020)

end

aux.RegisterCreatureEvent(1009120, 2, "c1009120_OnEnterCombat")


