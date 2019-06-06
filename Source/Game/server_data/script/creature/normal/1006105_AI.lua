--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006105_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30001)

end

aux.RegisterCreatureEvent(1006105, 2, "c1006105_OnEnterCombat")

--怪物死亡喊话
function c1006105_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 30003)	

end

aux.RegisterCreatureEvent(1006105, 4, "c1006105_OnDie")

