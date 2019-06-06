--怪物喊话脚本

--怪物死亡喊话
function c1005107_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 20007)	

end

aux.RegisterCreatureEvent(1005107, 4, "c1005107_OnDie")

