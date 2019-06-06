--怪物喊话脚本

-- 怪物进入战斗喊话
function c1007119_OnEnterCombat(MapID, InstanceID, CreatureID)
                  
        cre.MonsterSay(MapID, InstanceID, CreatureID, 40020)	

end

aux.RegisterCreatureEvent(1007119, 2, "c1007119_OnEnterCombat")


--怪物死亡喊话
function c1007119_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 40021)	

end

aux.RegisterCreatureEvent(1007119, 4, "c1007119_OnDie")

