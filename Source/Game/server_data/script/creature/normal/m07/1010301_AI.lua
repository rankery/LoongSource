--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010301_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50057)

end

aux.RegisterCreatureEvent(1010301, 2, "c1010301_OnEnterCombat")


--怪物死亡
function c1010301_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010301, 4, "c1010301_OnDie")
