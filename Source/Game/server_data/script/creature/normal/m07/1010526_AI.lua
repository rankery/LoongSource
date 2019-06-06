--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010526_OnEnterCombat(MapID, InstanceID, CreatureID)

       cre.MonsterSay(MapID, InstanceID, CreatureID, 50069)

end

aux.RegisterCreatureEvent(1010526, 2, "c1010526_OnEnterCombat")

--怪物死亡喊话
function c1010526_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 50070)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010526, 4, "c1010526_OnDie")


