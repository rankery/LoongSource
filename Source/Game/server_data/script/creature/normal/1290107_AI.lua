--怪物喊话脚本

-- 怪物进入战斗喊话
function c1290107_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 20107)
end

aux.RegisterCreatureEvent(1290107, 2, "c1290107_OnEnterCombat")


--怪物死亡喊话
function c1290107_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 20108)

end

aux.RegisterCreatureEvent(1290107, 4, "c1290107_OnDie")

