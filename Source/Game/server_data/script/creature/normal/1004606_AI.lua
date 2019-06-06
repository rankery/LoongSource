--怪物喊话脚本

-- 怪物进入战斗喊话
function c1004606_OnEnterCombat(MapID, InstanceID, CreatureID)
 
            cre.MonsterSay(MapID, InstanceID, CreatureID, 10055)
	
end

aux.RegisterCreatureEvent(1004606, 2, "c1004606_OnEnterCombat")


--怪物死亡喊话
function c1004606_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 10056)	

end

aux.RegisterCreatureEvent(1004606, 4, "c1004606_OnDie")

