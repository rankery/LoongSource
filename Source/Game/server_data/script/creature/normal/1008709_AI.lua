--怪物喊话脚本

-- 怪物进入战斗喊话
function c1008709_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 40085)
end

aux.RegisterCreatureEvent(1008709, 2, "c1008709_OnEnterCombat")


function c1008709_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	Increase_shili(MapID, InstanceID, RoleID, 1, 15, "jinjieshili_A")

end

aux.RegisterCreatureEvent(1008709, 4, "c1008709_OnDie")
