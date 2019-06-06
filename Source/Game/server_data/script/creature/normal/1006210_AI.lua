--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006210_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30012)
	
end

aux.RegisterCreatureEvent(1006210, 2, "c1006210_OnEnterCombat")



--怪物死亡喊话
function c1006210_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30013)	

end

aux.RegisterCreatureEvent(1006210, 4, "c1006210_OnDie")

