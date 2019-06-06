--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003178_OnEnterCombat(MapID, InstanceID, CreatureID)
          
          cre.MonsterSay(MapID, InstanceID, CreatureID, 76)
	
end

aux.RegisterCreatureEvent(1003178, 2, "c1003178_OnEnterCombat")


--怪物死亡喊话
function c1003178_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 77)	

end

aux.RegisterCreatureEvent(1003178, 4, "c1003178_OnDie")

