--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010303_OnEnterCombat(MapID, InstanceID, CreatureID)

      cre.MonsterSay(MapID, InstanceID, CreatureID, 50059)

end

aux.RegisterCreatureEvent(1010303, 2, "c1010303_OnEnterCombat")


--怪物死亡
function c1010303_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010303, 4, "c1010303_OnDie")
