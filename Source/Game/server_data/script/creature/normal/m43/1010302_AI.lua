--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010302_OnEnterCombat(MapID, InstanceID, CreatureID)

    cre.MonsterSay(MapID, InstanceID, CreatureID, 50058)

end

aux.RegisterCreatureEvent(1010302, 2, "c1010302_OnEnterCombat")
