--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006413_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30052)
	
end

aux.RegisterCreatureEvent(1006413, 2, "c1006413_OnEnterCombat")



--怪物死亡喊话
function c1006413_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30053)	

end

aux.RegisterCreatureEvent(1006413, 4, "c1006413_OnDie")

