--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006403_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30068)
	
end

aux.RegisterCreatureEvent(1006403, 2, "c1006403_OnEnterCombat")



--怪物死亡喊话
function c1006403_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30069)	

end

aux.RegisterCreatureEvent(1006403, 4, "c1006403_OnDie")

