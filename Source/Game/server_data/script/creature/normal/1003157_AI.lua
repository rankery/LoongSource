--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003157_OnEnterCombat(MapID, InstanceID, CreatureID)
 
             cre.MonsterSay(MapID, InstanceID, CreatureID, 58)
	
end

aux.RegisterCreatureEvent(1003157, 2, "c1003157_OnEnterCombat")


--怪物死亡喊话
function c1003157_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 60)	

end

aux.RegisterCreatureEvent(1003157, 4, "c1003157_OnDie")

