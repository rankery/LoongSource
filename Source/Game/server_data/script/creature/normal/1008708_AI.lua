--怪物喊话脚本


--怪物死亡喊话
function c1008708_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 40084)

end

aux.RegisterCreatureEvent(1008708, 4, "c1008708_OnDie")

