--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005103_OnEnterCombat(MapID, InstanceID, CreatureID)
       
             cre.MonsterSay(MapID, InstanceID, CreatureID, 20004)
	
end

aux.RegisterCreatureEvent(1005103, 2, "c1005103_OnEnterCombat")



--怪物死亡喊话
function c1005103_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 20005)	

end

aux.RegisterCreatureEvent(1005103, 4, "c1005103_OnDie")

