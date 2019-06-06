--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010201_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50053)

end

aux.RegisterCreatureEvent(1010201, 2, "c1010201_OnEnterCombat")


--怪物死亡
function c1010201_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010201, 4, "c1010201_OnDie")
