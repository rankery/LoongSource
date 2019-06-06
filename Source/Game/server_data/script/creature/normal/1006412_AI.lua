--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006412_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30050)
	
end

aux.RegisterCreatureEvent(1006412, 2, "c1006412_OnEnterCombat")


--怪物死亡喊话
function c1006412_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30051)

end

aux.RegisterCreatureEvent(1006412, 4, "c1006412_OnDie")

