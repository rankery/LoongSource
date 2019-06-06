--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006327_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30041)
	
end

aux.RegisterCreatureEvent(1006327, 2, "c1006327_OnEnterCombat")


--怪物死亡喊话
function c1006327_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30042)	

end

aux.RegisterCreatureEvent(1006327, 4, "c1006327_OnDie")

