--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006132_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30128)
	
end

aux.RegisterCreatureEvent(1006132, 2, "c1006132_OnEnterCombat")



--怪物死亡喊话
function c1006132_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30129)	

end

aux.RegisterCreatureEvent(1006132, 4, "c1006132_OnDie")

