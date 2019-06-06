--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003155_OnEnterCombat(MapID, InstanceID, CreatureID)
        
            cre.MonsterSay(MapID, InstanceID, CreatureID, 53)

end

aux.RegisterCreatureEvent(1003155, 2, "c1003155_OnEnterCombat")

--怪物死亡喊话
function c1003155_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 54)	

end

aux.RegisterCreatureEvent(1003155, 4, "c1003155_OnDie")

