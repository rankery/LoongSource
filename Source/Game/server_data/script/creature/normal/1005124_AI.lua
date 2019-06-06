--怪物喊话脚本


--怪物死亡喊话
function c1005124_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	          
       cre.MonsterSay(MapID, InstanceID, TargetID, 20012)

end

aux.RegisterCreatureEvent(1005124, 4, "c1005124_OnDie")

