--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010202_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50054)

end

aux.RegisterCreatureEvent(1010202, 2, "c1010202_OnEnterCombat")

--怪物死亡
function c1010202_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010202, 4, "c1010202_OnDie")

