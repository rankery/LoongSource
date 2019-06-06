--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010303_OnEnterCombat(MapID, InstanceID, CreatureID)

    cre.MonsterSay(MapID, InstanceID, CreatureID, 50059)

end

aux.RegisterCreatureEvent(1010303, 2, "c1010303_OnEnterCombat")
