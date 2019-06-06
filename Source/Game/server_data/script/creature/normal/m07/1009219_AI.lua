--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009219_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50029)

end

aux.RegisterCreatureEvent(1009219, 2, "c1009219_OnEnterCombat")


