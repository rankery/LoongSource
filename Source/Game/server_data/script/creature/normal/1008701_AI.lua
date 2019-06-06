--怪物喊话脚本

-- 怪物进入战斗喊话
function c1008701_OnEnterCombat(MapID, InstanceID, CreatureID)
       
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40075)
	
end

aux.RegisterCreatureEvent(1008701, 2, "c1008701_OnEnterCombat")


--怪物死亡喊话
function c1008701_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 40076)	

end

aux.RegisterCreatureEvent(1008701, 4, "c1008701_OnDie")

