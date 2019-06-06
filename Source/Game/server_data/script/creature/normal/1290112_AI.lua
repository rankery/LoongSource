--怪物喊话脚本

-- 怪物进入战斗喊话
function c1290112_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 30138)
end

aux.RegisterCreatureEvent(1290112, 2, "c1290112_OnEnterCombat")


--怪物死亡喊话
function c1290112_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 30139)

end

aux.RegisterCreatureEvent(1290112, 4, "c1290112_OnDie")

