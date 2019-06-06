--怪物喊话脚本

-- 怪物进入战斗喊话
function c1290105_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 10092)
end

aux.RegisterCreatureEvent(1290105, 2, "c1290105_OnEnterCombat")


--怪物死亡喊话
function c1290105_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 10093)

end

aux.RegisterCreatureEvent(1290105, 4, "c1290105_OnDie")

