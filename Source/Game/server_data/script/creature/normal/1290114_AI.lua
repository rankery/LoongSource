--怪物喊话脚本

-- 怪物进入战斗喊话
function c1290114_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 40097)
end

aux.RegisterCreatureEvent(1290114, 2, "c1290114_OnEnterCombat")


--怪物死亡喊话
function c1290114_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 40098)

end

aux.RegisterCreatureEvent(1290114, 4, "c1290114_OnDie")

