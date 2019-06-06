--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006122_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30065)
	
end

aux.RegisterCreatureEvent(1006122, 2, "c1006122_OnEnterCombat")

--怪物死亡喊话
function c1006122_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30067)

end

aux.RegisterCreatureEvent(1006122, 4, "c1006122_OnDie")

