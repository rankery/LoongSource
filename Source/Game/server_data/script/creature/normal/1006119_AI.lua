--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006119_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30006)
	
end

aux.RegisterCreatureEvent(1006119, 2, "c1006119_OnEnterCombat")


--怪物死亡喊话
function c1006119_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30007)	

end

aux.RegisterCreatureEvent(1006119, 4, "c1006119_OnDie")

