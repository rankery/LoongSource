--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006139_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30132)
	
end

aux.RegisterCreatureEvent(1006139, 2, "c1006139_OnEnterCombat")



--怪物死亡喊话
function c1006139_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30133)

end

aux.RegisterCreatureEvent(1006139, 4, "c1006139_OnDie")

