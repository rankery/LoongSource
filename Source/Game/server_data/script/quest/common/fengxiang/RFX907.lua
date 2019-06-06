--RFX907
--在216，233，160刷出烙印之灵（1531005），烙印之灵1531005被杀死后刷出2个烙印之灵1531009，杀死烙印之灵1531009后刷出3个烙印之灵1531010
--怪物死亡
function c1531005_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       unit.AddBuff(MapID, InstanceID, RoleID, 2016801, RoleID)
       unit.AddBuff(MapID, InstanceID, RoleID, 2016801, RoleID)
       map.MapCreateCreature(MapID, InstanceID, 1531009, 203, 140, 244)
       map.MapCreateCreature(MapID, InstanceID, 1531009, 226, 140, 244)

end

aux.RegisterCreatureEvent(1531005, 4, "c1531005_OnDie")

--怪物死亡
function c1531009_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       unit.AddBuff(MapID, InstanceID, RoleID, 2016801, RoleID)
       map.MapCreateCreature(MapID, InstanceID, 1531010, 199, 140, 232)
       map.MapCreateCreature(MapID, InstanceID, 1531010, 203, 140, 244)
       map.MapCreateCreature(MapID, InstanceID, 1531010, 226, 140, 244)

end

aux.RegisterCreatureEvent(1531009, 4, "c1531009_OnDie")

--怪物死亡
function c1531010_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       unit.AddBuff(MapID, InstanceID, RoleID, 2016801, RoleID)
end

aux.RegisterCreatureEvent(1531010, 4, "c1531010_OnDie")


--任务接取时函数
--function x11907_OnAccept(MapID, InstanceID, questID, ownerID, accepterID)
--	role.RoleGotoNewMap(MapID, InstanceID, ownerID, 1826990375, 171, 5, 32)
--end

--注册
--aux.RegisterQuestEvent(11907, 0, "x11907_OnAccept")

