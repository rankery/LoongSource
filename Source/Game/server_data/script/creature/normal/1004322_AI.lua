--怪物喊话脚本

-- 怪物进入战斗喊话
function c1004322_OnEnterCombat(MapID, InstanceID, CreatureID)
       
             cre.MonsterSay(MapID, InstanceID, CreatureID, 10042)
	
end

aux.RegisterCreatureEvent(1004322, 2, "c1004322_OnEnterCombat")



--怪物死亡喊话
function c1004322_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 10043)	

end

aux.RegisterCreatureEvent(1004322, 4, "c1004322_OnDie")

