--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010010_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 50036)

end

aux.RegisterCreatureEvent(1010010, 2, "c1010010_OnEnterCombat")


--怪物死亡喊话
function c1010010_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 50037)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end

aux.RegisterCreatureEvent(1010010, 4, "c1010010_OnDie")

