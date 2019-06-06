--怪物喊话脚本



--怪物死亡喊话
function c1005128_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	
             cre.MonsterSay(MapID, InstanceID, TargetID, 20014)	

end

aux.RegisterCreatureEvent(1005128, 4, "c1005128_OnDie")

