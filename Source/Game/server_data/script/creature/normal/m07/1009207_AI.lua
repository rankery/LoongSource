--怪物喊话脚本

-- 怪物进入战斗喊话
function c1009207_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50024)

end

aux.RegisterCreatureEvent(1009207, 2, "c1009207_OnEnterCombat")



